import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover
from esphome.const import CONF_ID
from .. import HUB_CHILD_SCHEMA, CONF_DOOYA_BRIDGE_ID, dooya_ns

DEPENDENCIES = ["dooya"]

CONF_ADDRESS = "address"

DooyaCover = dooya_ns.class_("DooyaCover", cg.Component, cover.Cover, cg.Parented)

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
    #paren = await cg.get_variable(config[CONF_DOOYA_BRIDGE_ID])
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_address(config[CONF_ADDRESS]))
    await cg.register_parented(var, config[CONF_DOOYA_BRIDGE_ID])
    #await cg.register_component(var, config)
    #cg.add(var.set_parent(paren))
    #await cover.register_cover(var, config)
