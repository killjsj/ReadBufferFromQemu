extends Sprite2D
@export var rd:ReaderClass

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


var button_state: int = 0
var wheel_accum:int  = 0

func _process(_delta):
	if wheel_accum != 0:
		rd.send_mouse_wheel(wheel_accum) # 一次发送累计值
		wheel_accum = 0
func _input(event):
	if event.is_echo():
		return
	if event is InputEventKey:
		if event.is_echo() or event.echo:
			return
		rd.send_key_event(event.keycode, event.pressed)
	elif event is InputEventMouseButton:
		match event.button_index:
			MOUSE_BUTTON_WHEEL_UP:
				wheel_accum += 1
			MOUSE_BUTTON_WHEEL_DOWN:
				wheel_accum -= 1
			MOUSE_BUTTON_WHEEL_LEFT:
				pass  # 暂不支持
			MOUSE_BUTTON_WHEEL_RIGHT:
				pass
			_:
				# 将 Godot 按钮索引转换为 QEMU 的位掩码
				var mask = 0
				match event.button_index:
					1: mask = 1 << 0   # 左键
					2: mask = 1 << 1   # 中键
					3: mask = 1 << 2   # 右键
					# 可扩展更多按钮
				if mask == 0: return

				if event.pressed:
					button_state |= mask	
				else:
					button_state &= ~mask
				var x = int(event.position.x)
				var y = int(event.position.y)
				rd.send_mouse_button_state(x,y,button_state)

	elif event is InputEventMouseMotion:
		var vp = get_viewport().get_visible_rect()
		var x = int(event.position.x)
		var y = int(event.position.y)
		rd.send_mouse_motion(x, y,
						  int(vp.size.x), int(vp.size.y))
		
