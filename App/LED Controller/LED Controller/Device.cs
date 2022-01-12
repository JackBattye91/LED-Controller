using System;
using System.Collections.Generic;
using System.Text;
using System.Text.Json;

namespace LED_Controller
{
    public class Device
    {
        public enum FEATURE_FLAGS
        {
            FEATURE_NONE = 0,
            FEATURE_ONOFF = 1,
            FEATURE_SINGLECOLOR = 2,
            FEATURE_SOLIDCOLOR = 4,
            FEATURE_MULTICOLOR = 8
        };

        public byte[] UUID { get; private set; }
        public string Name { get; set; }
        public bool On { get; set; }
        public int State { get; set; }
        protected int Features { get; private set; }
        public Dictionary<string, object> Values { get; set; }

        public Device()
        {
            UUID = new byte[16];
            On = false;
            State = 0;
            Features = 0;
            Values = new Dictionary<string, object>();
        }
        public bool HasFeature(FEATURE_FLAGS feature)
        {
            return ((int)Features & (int)feature > 0);
        }

        public override string ToString()
        {
            return JsonSerializer.Serialize<Device>(this);
        }
        public bool Parse(string jsonString)
        {
            try
            { 
                Device dev = JsonSerializer.Deserialize<Device>(jsonString);

                UUID = dev.UUID;
                Name = dev.Name;
                On = dev.On;
                State = dev.State;
                Features = new List<string>(dev.Features);
                Values = new Dictionary<string, object>(dev.Values);

                return true;
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
                return false;
            }
        }
    }
}
