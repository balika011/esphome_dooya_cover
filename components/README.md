```yaml
# example configuration:

uart:
  id: dooya_cover_uart
  tx_pin: GPIO15
  rx_pin: GPIO2
  baud_rate: 115200

dooya_bridge:
  id: dooya_bridge_1
  uart_id: dooya_cover_uart

cover:
  - platform: dooya_cover
    name: Rolling shutter 1
    dooya_bridge_id: dooya_bridge_1
    address: "AAA"

  - platform: dooya_cover
    name: Rolling shutter 2
    dooya_bridge_id: dooya_bridge_1
    address: "BBB"
```

