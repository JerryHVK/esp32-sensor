using System;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using AutoMapper;
using Esp32_MQTT.Server.Models.Domain;
using Esp32_MQTT.Server.Models.DTO;
using Esp32_MQTT.Server.Repositories;
using Microsoft.Extensions.Hosting;
using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Server;

public class MqttService : IHostedService
{
    private MqttFactory mqttFactory;
    private IMqttClient _mqttClient;
    private MqttClientOptions mqttClientOptions;
    private readonly string _brokerAddress = "localhost"; // Your broker address
    private readonly string _topic = "k"; // Your topic
    //public readonly IMax30102Repository max30102Repository;
    public readonly IMapper mapper;

    private readonly IServiceProvider _serviceProvider;

    public MqttService(IMapper mapper, IServiceProvider serviceProvider)
    {
        this._serviceProvider = serviceProvider;
        this.mapper = mapper;

        mqttFactory = new MqttFactory();
        _mqttClient = mqttFactory.CreateMqttClient();

        mqttClientOptions = new MqttClientOptionsBuilder().WithTcpServer(_brokerAddress).Build();
        
    }

    public async Task StartAsync(CancellationToken cancellationToken)
    {
        _mqttClient.ConnectedAsync += OnConnected;
        _mqttClient.ApplicationMessageReceivedAsync += OnMessageReceived;
        _mqttClient.DisconnectedAsync += OnDisconnected;

        await _mqttClient.ConnectAsync(mqttClientOptions, cancellationToken);
    }

    private async Task OnConnected(MqttClientConnectedEventArgs arg)
    {
        Console.WriteLine("Connected to broker.");
        var mqttSubscribeOptions = mqttFactory.CreateSubscribeOptionsBuilder()
                .WithTopicFilter(
                    f => {
                        f.WithTopic(_topic);
                    })
        .Build();

        await _mqttClient.SubscribeAsync(mqttSubscribeOptions, CancellationToken.None);
    }

    private async Task OnMessageReceived(MqttApplicationMessageReceivedEventArgs arg)
    {
        String data = Encoding.UTF8.GetString(arg.ApplicationMessage.PayloadSegment);

        int result;
        try
        {
            result = Int32.Parse(data);
            AddDataRequestDto addDataRequestDto = new AddDataRequestDto
            {
                HeartRate = result
            };

            
            using (var scope = _serviceProvider.CreateScope())
            {
                var repository = scope.ServiceProvider.GetRequiredService<IMax30102Repository>();

                // Use the repository here
                await repository.CreateAsync(mapper.Map<Max30102>(addDataRequestDto));
            }
        }
        catch (FormatException)
        {
            Console.WriteLine($"Unable to parse '{data}'");
        }
        await Task.CompletedTask;
    }

    private Task OnDisconnected(MqttClientDisconnectedEventArgs arg)
    {
        Console.WriteLine("Disconnected from broker.");
        return Task.CompletedTask;
    }

    public async Task StopAsync(CancellationToken cancellationToken)
    {
        await _mqttClient.DisconnectAsync();
    }

    public async Task PublishTopic()
    {
        await _mqttClient.PublishStringAsync("esp32", "toggle");
    }
}