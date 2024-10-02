namespace Esp32_MQTT.Server.Models.Domain
{
    public class Max30102
    {
        public int Id { get; set; }
        public int HeartRate { get; set; }
        public DateTime Time { get; set; } = DateTime.Now;

    }
}
