#pragma once
#include <cstdint>
#include <drivers/include/nrfx_saadc.h>
#include <array>
#include <numeric>
namespace Pinetime {
  namespace Controllers {

    /** A simple circular buffer that can be used to average 
     out the sensor values. The total capacity of the CircBuffer  
    is given as the template parameter N.
    */ 
    template <int N> 
    class CircBuffer {
      public:
        CircBuffer() : arr{}, sz{}, cap{N}, head{} {}
        /**
       insert member function overwrites the next data to the current 
      HEAD and moves the HEAD to the newly inserted value.
      */ 
        void insert(const int num) {
          head %= cap;
          arr[head++] = num;
          if (sz != cap) {
            sz++;
          }
        }

        int GetAverage() const {
          int sum = std::accumulate(arr.begin(), arr.end(), 0);
          return (sum / sz);
        }

      private:
        std::array<int, N> arr; /**< internal array used to store the values*/
        uint8_t sz; /**< The current size of the array.*/
        uint8_t cap; /**< Total capacity of the CircBuffer.*/
        uint8_t head; /**< The current head of the CircBuffer*/
    };

    class Battery {
      public:

        Battery();

        void Init();
        void Update();
        
        int PercentRemaining() const { return static_cast<int>(percentRemainingBuffer.GetAverage()); }

        float Voltage() const { return voltage; }

        bool IsCharging() const { return isCharging; }
        bool IsPowerPresent() const { return isPowerPresent; }

      private:
        static Battery *instance;
        nrf_saadc_value_t adc_buf[2];

        static constexpr uint8_t percentSamples = 5;
        CircBuffer<percentSamples> percentRemainingBuffer {};
        CircBuffer<percentSamples> voltageBuffer {};

        float voltage = 0.0f;
        int percentRemaining = -1;

        bool isCharging = false;
        bool isPowerPresent = false;
        
        void SaadcInit();

        void SaadcEventHandler(nrfx_saadc_evt_t const * p_event);
        static void adcCallbackStatic(nrfx_saadc_evt_t const *event);

        bool isReading = false;
        uint8_t samples = 0;
        uint8_t sigmoidal(uint16_t voltage, uint16_t minVoltage, uint16_t maxVoltage);
    };
  }
}