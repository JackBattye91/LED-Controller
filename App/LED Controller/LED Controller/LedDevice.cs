using System;
using System.Collections.Generic;
using System.Text;

namespace LED_Controller
{
    public abstract class LedDevice : Device
    {
        public LedDevice() : base()
        {
        }
        public LedDevice(Guid uuid) : base(uuid)
        {
        }
    }
}
