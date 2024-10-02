using Esp32_MQTT.Server.Data;
using Esp32_MQTT.Server.Models.Domain;
using Microsoft.EntityFrameworkCore;

namespace Esp32_MQTT.Server.Repositories
{
    public class SQLMax30102Repository : IMax30102Repository
    {
        public readonly Max30102DbContext dbContext;
        public SQLMax30102Repository(Max30102DbContext dbContext)
        {
            this.dbContext = dbContext;
        }
        public async Task<Max30102> CreateAsync(Max30102 max30102)
        {
            await dbContext.max30102s.AddAsync(max30102);
            await dbContext.SaveChangesAsync();
            return max30102;    
        }

        public async Task<Max30102?> GetNewAsync()
        {
            return await dbContext.max30102s
                .OrderByDescending(x => x.Id)
                .FirstOrDefaultAsync();
        }
    }
}
