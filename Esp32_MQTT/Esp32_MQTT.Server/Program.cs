using Esp32_MQTT.Server;
using Esp32_MQTT.Server.Data;
using Esp32_MQTT.Server.Mappings;
using Esp32_MQTT.Server.Repositories;
using Microsoft.EntityFrameworkCore;

using MQTTnet.AspNetCore;
using MQTTnet.AspNetCore.Routing;

var builder = WebApplication.CreateBuilder(args);

// Add services to the container.

builder.Services.AddHostedService<MqttService>();

builder.Services.AddControllers();
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

builder.Services.AddDbContext<Max30102DbContext>(options =>
options.UseSqlServer(builder.Configuration.GetConnectionString("Max30102ConnectionString")));

builder.Services.AddScoped<IMax30102Repository, SQLMax30102Repository>();

builder.Services.AddAutoMapper(typeof(AutoMapperProfiles));

//builder.Services.AddScoped<IMqtt, LocalMqttService>();


var app = builder.Build();

app.UseDefaultFiles();
app.UseStaticFiles();

// Configure the HTTP request pipeline.
//if (app.Environment.IsDevelopment())
//{
//    app.UseSwagger();
//    app.UseSwaggerUI();
//}

app.UseSwagger();
app.UseSwaggerUI();

app.UseHttpsRedirection();


app.UseAuthorization();

app.MapControllers();

app.MapFallbackToFile("/index.html");

app.Run();
