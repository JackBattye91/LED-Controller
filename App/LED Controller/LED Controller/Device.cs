using System;
using System.Collections.Generic;
using System.Text;
using System.Text.Json;

namespace LED_Controller
{
    public class Device
    {
        public Guid UUID { get; private set; }
        public string Name { get; set; }
        public bool On { get; set; }
        public int State { get; set; }
        protected List<string> Features { get; set; }
        public Dictionary<string, object> Values { get; set; }

        public Device()
        {
            UUID = Guid.NewGuid();
            Features = new List<string>();
            Values = new Dictionary<string, object>();
        }
        public Device(Guid uuid)
        {
            UUID = uuid;
            Features = new List<string>();
            Values = new Dictionary<string, object>();
        }

        public void AddFeature(string feature)
        {
            if (HasFeature(feature))
                return;
            
            Features.Add(feature.ToLower());
        }
        public bool HasFeature(string feature)
        {
            return Features.Contains(feature.ToLower());
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
