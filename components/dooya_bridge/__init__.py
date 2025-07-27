import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

MULTI_CONF = True

CONF_DOOYA_BRIDGE_ID = "dooya_bridge_id"

dooya_bridge_ns = cg.esphome_ns.namespace("dooya_bridge")
DooyaBridge = dooya_bridge_ns.class_("DooyaBridge", cg.Component)

HUB_CHILD_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_DOOYA_BRIDGE_ID): cv.use_id(DooyaBridge),
    }
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DooyaBridge),
    }
).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await uart.register_uart_device(var, config)
    await cg.register_component(var, config)