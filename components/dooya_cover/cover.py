import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, cover
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

CONF_ADDRESS = "address"

dooya_cover_ns = cg.esphome_ns.namespace("dooya_cover")
DooyaCover = dooya_cover_ns.class_("DooyaCover", cg.Component, cover.Cover, uart.UARTDevice)

def validate_address(config):
    if len(config[CONF_ADDRESS]) != 3:
         raise cv.Invalid("Address MUST be 3 alphanumeric characters.")
    return config

CONFIG_SCHEMA = cv.All(
    cover.cover_schema(DooyaCover)
    .extend({cv.Required(CONF_ADDRESS): cv.alphanumeric})
    .extend(uart.UART_DEVICE_SCHEMA),
    validate_address
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    cg.add(var.set_address(config[CONF_ADDRESS]))
    await cover.register_cover(var, config)
