using System;
using System.Collections.Generic;
using System.Text;
using System.Text.Json;

namespace LED_Controller
{
    public abstract class Device
    {
        public Guid UUID { get; private set; }
        public string Name { get; set; }
        public int State { get; set; }
        protected List<string> Features { get; set; }
        public Dictionary<string, string> Values { get; set; }

        public Device()
        {
            UUID = Guid.NewGuid();
            Features = new List<string>();
            Values = new Dictionary<string, string>();
        }
        public Device(Guid uuid)
        {
            UUID = uuid;
            Features = new List<string>();
            Values = new Dictionary<string, string>();
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

        public static string ToString()
        {
            return JsonSerializer.Serialize<Device>(this);
        }
        public static Device Parse(string jsonString)
        {
            return JsonSerializer.Deserialize<Device>(jsonString);
        }
    }
}
