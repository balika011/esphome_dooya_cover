import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover
from esphome.const import CONF_ID
from .. import dooya_ns, DOOYA_CHILD_SCHEMA, validate_address, DooyaComponent, register_component

DEPENDENCIES = ["dooya"]

DooyaCover = dooya_ns.class_("DooyaCover", DooyaComponent, cover.Cover)

CONFIG_SCHEMA = cv.All(
    cover.cover_schema(DooyaCover)
    .extend(DOOYA_CHILD_SCHEMA),
    validate_address
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await register_component(var, config)
    await cover.register_cover(var, config)
