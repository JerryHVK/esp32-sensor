using Esp32_MQTT.Server.Models.Domain;

namespace Esp32_MQTT.Server.Repositories
{
    public interface IMax30102Repository
    {
    
        Task<Max30102?> GetNewAsync();

        Task<Max30102> CreateAsync(Max30102 max30102);
    }
}
