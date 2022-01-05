using System;
using System.Collections.Generic;
using System.Text;

namespace LED_Controller
{
    public abstract class Device
    {
        public Guid UUID { get; private set; }
        public string Name { get; set; }
        public List<string> Features { get; set; }

        public Device()
        {
            UUID = Guid.NewGuid();
            Features = new List<string>();
        }
        public Device(Guid uuid)
        {
            UUID = uuid;
            Features = new List<string>();
        }

        public override string ToString()
        {
            return "{ " + $"'UUID' : {UUID}, 'Name' : {Name}" + "}"; 
        }
    }
}
