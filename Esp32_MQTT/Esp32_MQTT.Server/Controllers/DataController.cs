using AutoMapper;
using Esp32_MQTT.Server.Models.DTO;
using Esp32_MQTT.Server.Repositories;
using Microsoft.AspNetCore.Mvc;
using Esp32_MQTT.Server.Models.Domain;

namespace Esp32_MQTT.Server.Controllers
{
    [Route("[controller]")]
    [ApiController]
    public class DataController : ControllerBase
    {
        private readonly IMax30102Repository max30102Repository;
        private readonly IMapper mapper;

        public DataController(IMax30102Repository max30102Repository, IMapper mapper)
        {
            this.max30102Repository = max30102Repository;
            this.mapper = mapper;
        }


        // Get the data in database
        //[HttpGet]
        //public async Task<IActionResult> GetNew()
        //{
        //    var max30102 = await max30102Repository.GetNewAsync();
        //    if (max30102 == null)
        //    {
        //        return NotFound();
        //    }

        //    return Ok(mapper.Map<GetDataRequestDto>(max30102));
        //}


        // Save the data to database
        // POST: /data
        [HttpPost]
        public async Task<IActionResult> SaveToDb([FromBody] AddDataRequestDto addDataRequestDto)
        {
            var data = mapper.Map<Max30102>(addDataRequestDto);

            await max30102Repository.CreateAsync(data);

            return Ok(mapper.Map<GetDataRequestDto>(data));
        }
    }
}
