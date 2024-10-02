using Esp32_MQTT.Server.Models.Domain;
using Microsoft.EntityFrameworkCore;

namespace Esp32_MQTT.Server.Data
{
    public class Max30102DbContext: DbContext
    {
        public Max30102DbContext(DbContextOptions dbContextOptions): base(dbContextOptions)
        {
            
        }

        public DbSet<Max30102> max30102s { get; set; }  
    }
}
