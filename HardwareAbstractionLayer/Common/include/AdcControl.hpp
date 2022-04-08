///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: CLOSED
 *
 * @author: denis@schoener-one.de
 * @date:   2022-03-08
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AdcInput.hpp"
#include "IAdcControl.hpp"
#include "IGpioPin.hpp"

namespace wsadhat
{
class SpiControl;
class AdcHat;
}  // namespace wsadhat

namespace sugo::hal
{
/**
 * @brief Class for contolling ADC input devices
 *
 */
class AdcControl : public IAdcControl
{
public:
    AdcControl(const Identifier& id, IGpioPin& ioAdcHatCs, IGpioPin& ioAdcHatRst,
               IGpioPin& ioAdcHatRdy)
        : IAdcControl(id),
          m_ioAdcHatCs(ioAdcHatCs),
          m_ioAdcHatRst(ioAdcHatRst),
          m_ioAdcHatRdy(ioAdcHatRdy)
    {
    }
    ~AdcControl() override;

    bool init(const IConfiguration& configuration) override;

    void finalize();

    const AdcInputMap& getAdcInputMap() override
    {
        return m_adcInputMap;
    }

private:
    IGpioPin&            m_ioAdcHatCs;
    IGpioPin&            m_ioAdcHatRst;
    IGpioPin&            m_ioAdcHatRdy;
    AdcInputMap          m_adcInputMap;
    AdcFilterMap         m_adcFilterMap;
    std::string          m_device;
    wsadhat::SpiControl* m_spiControl = nullptr;
    wsadhat::AdcHat*     m_adcHat     = nullptr;
};

}  // namespace sugo::hal
