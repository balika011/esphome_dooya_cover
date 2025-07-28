import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID
from esphome.automation import maybe_simple_id

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

DooyaBridgePairingAction = at581x_ns.class_("DooyaBridgePairingAction", automation.Action)

@automation.register_action(
    "dooya_bridge.pairing",
    DooyaBridgePairingAction,
    maybe_simple_id(
        {
            cv.Required(CONF_ID): cv.use_id(DooyaBridge),
        }
    ),
)
async def dooya_bridge_pairing_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])

    return var