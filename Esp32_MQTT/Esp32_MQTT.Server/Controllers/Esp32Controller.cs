using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using MQTTnet.Client;
using MQTTnet.Protocol;
using MQTTnet;

namespace Esp32_MQTT.Server.Controllers
{
    [Route("[controller]")]
    [ApiController]
    public class Esp32Controller : ControllerBase
    {
        Mqtt mqtt;
        public Esp32Controller()
        {
            this.mqtt = new Mqtt();
        }


        [HttpGet]
        [Route("toggleLight")]
        public async Task<IActionResult> ToggleLight()
        {
            if (!mqtt.IsConnected())
            {
                await mqtt.ClientConnect("broker.emqx.io", 1883);
            }

            await mqtt.ClientPublish("toggleLight", "khang/esp32");

            return Ok();

        }
    }

    class Mqtt
    {
        IMqttClient _mqttClient;
        MqttFactory _factory;

        public async Task ClientConnect(string broker, int port)
        {
            // Create a MQTT client factory
            _factory = new MqttFactory();

            // Create a MQTT client instance
            var mqttClient = _factory.CreateMqttClient();

            // Create MQTT client options
            var options = new MqttClientOptionsBuilder()
                .WithTcpServer(broker, port) // MQTT broker address and port
                .WithCleanSession()
                .Build();


            var connectResult = await mqttClient.ConnectAsync(options);
            if (connectResult.ResultCode == MqttClientConnectResultCode.Success)
            {
                Console.WriteLine("Connected to MQTT broker successfully.");
            }
            else
            {
                Console.WriteLine("Failed to connected to MQTT broker.");
            }
            _mqttClient = mqttClient;
        }

        public Boolean IsConnected()
        {
            return _mqttClient != null;
        }
        
        public async Task ClientPublish(string message, string topic)
        {
            // convert object to string
            var messageForm = new MqttApplicationMessageBuilder()
                    .WithTopic(topic)
                    .WithPayload(message)
                    .WithQualityOfServiceLevel(MqttQualityOfServiceLevel.AtLeastOnce)
                    .WithRetainFlag()
                    .Build();
            // publish the message
            await _mqttClient.PublishAsync(messageForm);
        }

        //public async Task ClientSubscribe(string topic)
        //{
        //    // subscribe to a topic
        //    await _mqttClient.SubscribeAsync(topic);

        //    // Callback function when a message is received
        //    _mqttClient.ApplicationMessageReceivedAsync += e =>
        //    {
        //        Data data = JsonConvert.DeserializeObject<Data>(Encoding.UTF8.GetString(e.ApplicationMessage.Payload));
        //        Console.WriteLine($"\nDeviceId: {data.DeviceId}");
        //        Console.WriteLine($"packet_no: {data.packet_no}");
        //        Console.WriteLine($"temperature: {data.temperature}");
        //        Console.WriteLine($"humidity: {data.humidity}\n");
        //        return Task.CompletedTask;
        //    };
        //}
    }
}
