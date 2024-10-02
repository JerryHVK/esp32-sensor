using AutoMapper;
using Esp32_MQTT.Server.Models.Domain;
using Esp32_MQTT.Server.Models.DTO;

namespace Esp32_MQTT.Server.Mappings
{
    public class AutoMapperProfiles : Profile
    {
        public AutoMapperProfiles()
        {
            CreateMap<Max30102, AddDataRequestDto>().ReverseMap();
            CreateMap<Max30102, GetDataRequestDto>().ReverseMap();
        }
    }
}
