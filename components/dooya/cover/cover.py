import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover
from esphome.const import CONF_ID
from esphome.components.dooya_bridge import HUB_CHILD_SCHEMA, CONF_DOOYA_BRIDGE_ID

DEPENDENCIES = ["dooya_bridge"]

CONF_ADDRESS = "address"

dooya_cover_ns = cg.esphome_ns.namespace("dooya_cover")
DooyaCover = dooya_cover_ns.class_("DooyaCover", cg.Component, cover.Cover)

def validate_address(config):
    if len(config[CONF_ADDRESS]) != 3:
         raise cv.Invalid("Address MUST be 3 alphanumeric characters.")
    return config

CONFIG_SCHEMA = cv.All(
    cover.cover_schema(DooyaCover)
    .extend(HUB_CHILD_SCHEMA)
    .extend({cv.Required(CONF_ADDRESS): cv.alphanumeric}),
    validate_address
)

async def to_code(config):
    paren = await cg.get_variable(config[CONF_DOOYA_BRIDGE_ID])
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_parent(paren))
    await cover.register_cover(var, config)
