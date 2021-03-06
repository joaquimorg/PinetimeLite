#include "FileService.h"
#include <cstring>
#include "drivers/SpiNorFlash.h"
#include "systemtask/SystemTask.h"
#include <string>
#include <lvgl/lvgl.h>

using namespace Pinetime::Controllers;

// provide linktime space for constants
constexpr ble_uuid128_t FileService::serviceUuid;
constexpr ble_uuid128_t FileService::fileDataCharacteristicUuid;
constexpr ble_uuid128_t FileService::fileControlCharacteristicUuid;


namespace {
  int FileServiceCallback(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg) {
    auto fileService = static_cast<FileService *>(arg);
    return fileService->OnServiceData(conn_handle, attr_handle, ctxt);
  }

  void TimeoutTimerCallback(TimerHandle_t xTimer) {
      auto fileService = static_cast<FileService *>(pvTimerGetTimerID(xTimer));
      fileService->OnTimeout();
  }
}

FileService::FileService(Pinetime::System::SystemTask &systemTask,
                       Pinetime::Controllers::Ble &bleController,
                       Pinetime::Drivers::SpiNorFlash &spiNorFlash)

: mSystemTask{systemTask}, bleController{bleController}, spiNorFlash{spiNorFlash}, spiFlash{spiNorFlash},

  mCharacteristicDefinitions {
      { .uuid = reinterpret_cast<const ble_uuid_t*>(&fileDataCharacteristicUuid),
        .access_cb = FileServiceCallback,
        .arg = this,
        .flags = (BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_READ),
        .val_handle = nullptr },

      { .uuid = reinterpret_cast<const ble_uuid_t*>(&fileControlCharacteristicUuid),
        .access_cb = FileServiceCallback,
        .arg = this,
        .flags = (BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_NOTIFY),
        .val_handle = nullptr },

      { 0 /* null terminator */ }
  },

  mServiceDefinitions {
      { .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = reinterpret_cast<const ble_uuid_t*>(&serviceUuid),
        .characteristics = mCharacteristicDefinitions
      },

      { 0 /* null terminator */ }
  } {
  timeoutTimer = xTimerCreate ("notificationTimer", 10000, pdFALSE, this, TimeoutTimerCallback);
}

void FileService::Init() {
  int res;
  res = ble_gatts_count_cfg(mServiceDefinitions);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(mServiceDefinitions);
  ASSERT(res == 0);

}

int FileService::OnServiceData(uint16_t connectionHandle, uint16_t attributeHandle, ble_gatt_access_ctxt *context) {

  mtuSize = ble_att_preferred_mtu();
  //mtuSize = ble_att_mtu(connectionHandle);

  if(bleController.IsFirmwareUpdating()){
    xTimerStart(timeoutTimer, 0);
  }

  ble_gatts_find_chr((ble_uuid_t *) &serviceUuid, (ble_uuid_t *) &fileDataCharacteristicUuid, nullptr,
                     &fileDataCharacteristicHandle);

  ble_gatts_find_chr((ble_uuid_t *) &serviceUuid, (ble_uuid_t *) &fileControlCharacteristicUuid, nullptr,
                     &fileControlCharacteristicHandle);


  if (attributeHandle == fileDataCharacteristicHandle) {
    switch(context->op) {
      case BLE_GATT_ACCESS_OP_WRITE_CHR: {
        return WritePacketHandler(connectionHandle, context->om);
      }
      case BLE_GATT_ACCESS_OP_READ_CHR: {
        return HandleRead(context);
      }
      default:
        // nimble should never have called us
        NRF_LOG_INFO("[DFU] Unknown Characteristic : %d", attributeHandle);
        return BLE_ATT_ERR_UNLIKELY;
    }

  } else if (attributeHandle == fileControlCharacteristicHandle) {
    if (context->op == BLE_GATT_ACCESS_OP_WRITE_CHR)
      return ControlPointHandler(connectionHandle, context->om);
    else return 0;
  } else {
    NRF_LOG_INFO("[FileService] Unknown Characteristic : %d", attributeHandle);
    return 0;
  }

  return BLE_ATT_ERR_UNLIKELY;
}

int FileService::ControlPointHandler(uint16_t connectionHandle, os_mbuf *om) {
  auto opcode = static_cast<Opcodes>(om->om_data[0]);

  switch (opcode) {
    case Opcodes::COMMAND_SEND_FIRMWARE_INFO: {
      if (state != States::Idle && state != States::Start) {
        NRF_LOG_INFO("[FileService] -> Start requested, but we are not in Idle state");
        return 0;
      }
      if (state == States::Start) {
        NRF_LOG_INFO("[FileService] -> Start requested, but we are already in Start state");
        return 0;
      }
      uint8_t data[2];

      // Validation of size and erase flash

      fileSize = om->om_data[1] + (om->om_data[2] << 8) + (om->om_data[3] << 16) + (om->om_data[4] << 24);
      bytesReceived = 0;

      if (fileSize > 0) {
        data[0] = 0x01;

        if ( om->om_data[5] == 0x01 ) {
          bleController.FWType(Pinetime::Controllers::Ble::FirmwareType::RES);
          spiFlash.Init(mtuSize, fileSize, SpiFlash::FlashType::RES);
        } else if ( om->om_data[5] == 0x02 ) {
          if ( fileSize > 475136 ) {
            // Notify Error !
            data[1] = (uint8_t)Opcodes::COMMAND_FIRMWARE_ERROR;
            NotificationSend(connectionHandle, fileControlCharacteristicHandle, data, sizeof(data));
            Reset();
            return 0;
          }
          bleController.FWType(Pinetime::Controllers::Ble::FirmwareType::FW);
          spiFlash.Init(mtuSize, fileSize, SpiFlash::FlashType::FW);
        } else if ( om->om_data[5] == 0x03 ) {
          if ( fileSize > 32800 ) {
            // Notify Error !
            data[1] = (uint8_t)Opcodes::COMMAND_FIRMWARE_ERROR;
            NotificationSend(connectionHandle, fileControlCharacteristicHandle, data, sizeof(data));
            Reset();
            return 0;
          }
          bleController.FWType(Pinetime::Controllers::Ble::FirmwareType::BOT);
          spiFlash.Init(mtuSize, fileSize, SpiFlash::FlashType::BOT);
        }

        bleController.StartFirmwareUpdate();
        bleController.State(Pinetime::Controllers::Ble::FirmwareUpdateStates::FormatFlash);
        bleController.FirmwareUpdateTotalBytes(fileSize);
        bleController.FirmwareUpdateCurrentBytes(0);
        // Send task to open app
        mSystemTask.PushMessage(Pinetime::System::SystemTask::Messages::OnResourceUpdateStart);

        spiFlash.Erase();
        bleController.State(Pinetime::Controllers::Ble::FirmwareUpdateStates::Running);

        // Notify to send Firmware !
        data[1] = (uint8_t)Opcodes::COMMAND_FIRMWARE_INIT;
        NotificationSend(connectionHandle, fileControlCharacteristicHandle, data, sizeof(data));

        state = States::Start;
      }
      return 0;
    }

    case Opcodes::COMMAND_FIRMWARE_START_DATA: {

      if (state != States::Start) {
        NRF_LOG_INFO("[FileService] -> Receive firmware image requested, but we are not in Start");
        return 0;
      }

      state = States::Data;

      return 0;
    }

    case Opcodes::COMMAND_FIRMWARE_CHECKSUM: {

      if (state != States::Validate) {
        NRF_LOG_INFO("[FileService] -> Validate firmware image requested, but we are not in Data state %d", state);
        return 0;
      }

      uint16_t crc = om->om_data[1] + (om->om_data[2] << 8);

      // verify Checksum
      if(spiFlash.CalculateCrc() == crc){

        bleController.State(Pinetime::Controllers::Ble::FirmwareUpdateStates::Validated);
        bleController.StopFirmwareUpdate();

        // Notify result of checksum
        uint8_t data[2];
        data[0] = 0x01;
        data[1] = (uint8_t)Opcodes::COMMAND_FIRMWARE_OK;

        NotificationSend(connectionHandle, fileControlCharacteristicHandle, data, sizeof(data));

        mSystemTask.PushMessage(Pinetime::System::SystemTask::Messages::OnResourceUpdateEnd);

        xTimerStop(timeoutTimer, 0);

        state = States::Idle;
      } else {
        // Notify result of checksum
        uint8_t data[2];
        data[0] = 0x01;
        data[1] = (uint8_t)Opcodes::COMMAND_FIRMWARE_CHECKSUM_ERR;

        NotificationSend(connectionHandle, fileControlCharacteristicHandle, data, sizeof(data));
        bleController.State(Pinetime::Controllers::Ble::FirmwareUpdateStates::Error);

      }

      return 0;
    }

    case Opcodes::COMMAND_FIRMWARE_UPDATE_SYNC: {

      return 0;
    }

    case Opcodes::COMMAND_FIRMWARE_END_DATA: {
      if (state != States::Data) {
        NRF_LOG_INFO("[FileService] -> Receive firmware end data requested, but we are not in Data");
        return 0;
      }

      if ( spiFlash.IsComplete() ) {
        // Notify end
        uint8_t data[2];
        data[0] = 0x01;
        data[1] = (uint8_t)Opcodes::COMMAND_FIRMWARE_END_DATA;

        NotificationSend(connectionHandle, fileControlCharacteristicHandle, data, sizeof(data));

        bleController.State(Pinetime::Controllers::Ble::FirmwareUpdateStates::Validated);
        xTimerStop(timeoutTimer, 0);
        state = States::Validate;
      } else {
        // Notify result size error
        uint8_t data[2];
        data[0] = 0x01;
        data[1] = (uint8_t)Opcodes::COMMAND_FIRMWARE_ERROR;

        NotificationSend(connectionHandle, fileControlCharacteristicHandle, data, sizeof(data));
        bleController.State(Pinetime::Controllers::Ble::FirmwareUpdateStates::Error);
      }

      return 0;
    }

    case Opcodes::COMMAND_SCREEN_SHOT: {
        ScreenShot(connectionHandle);
        readSCRPos = 0;
      return 0;
    }
    case Opcodes::COMMAND_SCREEN_SHOT_GET: {
        //ScreenShot(connectionHandle);
        uint8_t data[2];
        data[0] = 0x01;
        data[1] = (uint8_t)Opcodes::COMMAND_SCREEN_SHOT_GET;

        NotificationSend(connectionHandle, fileControlCharacteristicHandle, data, sizeof(data));
        readSCRPos = 0;
      return 0;
    }

    default:
      return 0;
  }
}

int FileService::HandleRead(ble_gatt_access_ctxt* context) {
  uint8_t rgbdata[240];
  uint32_t readOffset = 0x040000;
  //uint32_t totalSize = (240*240) * 2;
  
  //for (size_t rPos = 0; rPos < totalSize; rPos += sizeof(rgbdata)) {
    spiNorFlash.Read(readOffset + readSCRPos, rgbdata, sizeof(rgbdata));
    os_mbuf_append(context->om, rgbdata, sizeof(rgbdata));
    readSCRPos += sizeof(rgbdata);

    //if ( readSCRPos >= totalSize) return BLE_ATT_ERR_INSUFFICIENT_RES;
  //}
  return 0;
}


static void screenshot_disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
  auto* obj = static_cast<FileService*>(disp_drv->user_data);
  obj->FlushScreenShot(area, color_p);
  lv_disp_flush_ready(disp_drv);
}

void FileService::ScreenShot(uint16_t connectionHandle) {
  
  lv_disp_drv_t driver;
  lv_disp_t *system_disp;
  uint32_t fileSize = (240*240) * 2;

  readSCRPos = 0;
  
  spiFlash.Init(mtuSize, fileSize, SpiFlash::FlashType::SCR);
  spiFlash.Erase();

  system_disp = lv_disp_get_default();
  driver.user_data = system_disp->driver.user_data;
  driver.flush_cb = system_disp->driver.flush_cb;  
  system_disp->driver.flush_cb = screenshot_disp_flush;
  system_disp->driver.user_data = this;
  lv_obj_invalidate( lv_scr_act() );
  lv_refr_now( system_disp );
  system_disp->driver.user_data = driver.user_data;
  system_disp->driver.flush_cb = driver.flush_cb;

  uint8_t data[2];
  data[0] = 0x01;
  data[1] = (uint8_t)Opcodes::COMMAND_SCREEN_SHOT;

  NotificationSend(connectionHandle, fileControlCharacteristicHandle, data, sizeof(data));
}

void FileService::FlushScreenShot(const lv_area_t *area, lv_color_t *color_p) {
  lv_coord_t x, y;
  uint16_t *data = (uint16_t *)color_p;
  uint8_t rgbdata[2];
  uint32_t size = 0;
  for(y = area->y1; y <= area->y2; y++) {
    for(x = area->x1; x <= area->x2; x++) {
      
      rgbdata[0] = (uint8_t)((*data) >> 8);
      rgbdata[1] = (uint8_t)((*data) & 0xff);
      data++;
      spiFlash.Append(rgbdata, sizeof(rgbdata));
      size +=2;
      if (size > ((240*240) * 2)) return;
    }
  }
}

int FileService::WritePacketHandler(uint16_t connectionHandle, os_mbuf *om) {
  switch (state) {
    case States::Data: {

      spiFlash.Append(om->om_data, om->om_len);

      bytesReceived += om->om_len;
      bleController.FirmwareUpdateCurrentBytes(bytesReceived);

      return 0;
    }

    default:
      // Invalid state
      return 0;
  }
  return 0;
}


void FileService::NotificationSend(uint16_t connection, uint16_t charactHandle, const uint8_t *data, const size_t s) {
  auto *om = ble_hs_mbuf_from_flat(data, s);
  auto ret = ble_gattc_notify_custom(connection, charactHandle, om);
  ASSERT(ret == 0);
}


void FileService::Reset() {
  Restart();
  mSystemTask.PushMessage(Pinetime::System::SystemTask::Messages::OnResourceUpdateEnd);
}


void FileService::Restart() {
  state = States::Idle;
  bytesReceived = 0;
  bleController.State(Pinetime::Controllers::Ble::FirmwareUpdateStates::Error);
  bleController.StopFirmwareUpdate();
}

void FileService::OnTimeout() {

  Reset();
}

void FileService::SpiFlash::Init(size_t chunkSize, size_t totalSize, FlashType flashType) {
  //if(chunkSize != 20) return;
  this->chunkSize = chunkSize;
  this->totalSize = totalSize;
  this->ready = true;
  this->flashType = flashType;
  if ( this->flashType == FlashType::RES ) {
    writeOffset = 0x0B4000;
  } else if ( this->flashType == FlashType::FW ) {
    writeOffset = 0x040000;
  } else if ( this->flashType == FlashType::SCR ) {
    writeOffset = 0x040000;
  } else if ( this->flashType == FlashType::BOT ) {
    writeOffset = 0x000000;
  }

}

void FileService::SpiFlash::Append(uint8_t *data, size_t size) {

  if(!ready) return;
  ASSERT(size <= this->chunkSize);

  std::memcpy(tempBuffer + bufferWriteIndex, data, size);
  bufferWriteIndex += size;

  if(bufferWriteIndex == bufferSize) {
    if ( !demoMode ) {
      spiNorFlash.Write(writeOffset + totalWriteIndex, tempBuffer, bufferWriteIndex);
    }
    totalWriteIndex += bufferWriteIndex;
    bufferWriteIndex = 0;
  }

  if(bufferWriteIndex > 0 && totalWriteIndex + bufferWriteIndex == totalSize) {
    if ( !demoMode ) {
      spiNorFlash.Write(writeOffset + totalWriteIndex, tempBuffer, bufferWriteIndex);
    }
    totalWriteIndex += bufferWriteIndex;
    if ( this->flashType == FlashType::FW ) {
      if (totalSize < maxSize)
        WriteMagicNumber();
    }
  }
}

void FileService::SpiFlash::WriteMagicNumber() {
  uint32_t magic[4] = { // TODO When this variable is a static constexpr, the values written to the memory are not correct. Why?
          0xf395c277,
          0x7fefd260,
          0x0f505235,
          0x8079b62c,
  };
  uint32_t offset = writeOffset + (maxSize - (4 * sizeof(uint32_t)));
  spiNorFlash.Write(offset, reinterpret_cast<const uint8_t *>(magic), 4 * sizeof(uint32_t));
}

void FileService::SpiFlash::Erase() {
  if ( demoMode ) return;
  for (size_t erased = 0; erased < totalSize; erased += 0x1000) {
    spiNorFlash.SectorErase(writeOffset + erased);
  }
}

uint16_t FileService::SpiFlash::CalculateCrc() {
  uint32_t chunkSize = 200;
  size_t currentOffset = 0;
  uint16_t crc = 0;

  bool first = true;
  while (currentOffset < totalSize) {
    uint32_t readSize = (totalSize - currentOffset) > chunkSize ? chunkSize : (totalSize - currentOffset);

    spiNorFlash.Read(writeOffset + currentOffset, tempBuffer, readSize);
    if (first) {
      crc = ComputeCrc(tempBuffer, readSize, NULL);
      first = false;
    } else
      crc = ComputeCrc(tempBuffer, readSize, &crc);
    currentOffset += readSize;
  }

  return crc;

}

uint16_t FileService::SpiFlash::ComputeCrc(uint8_t const *p_data, uint32_t size, uint16_t const *p_crc) {
  uint16_t crc = (p_crc == NULL) ? 0xFFFF : *p_crc;

  for (uint32_t i = 0; i < size; i++) {
    crc = (crc >> 8) | (crc << 8);
    crc ^= p_data[i];
    crc ^= ((uint8_t) crc) >> 4;
    crc ^= crc << 12;
    crc ^= (crc & 0xFF) << 5;
  }

  return crc;
}

bool FileService::SpiFlash::IsComplete() {
  if(!ready) return false;
  return totalWriteIndex == totalSize;
}