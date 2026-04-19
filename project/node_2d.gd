extends ReaderClass

# 用于存放动态生成的 Sprite2D 节点
var screen_sprites: Array[Sprite2D] = []

func _ready() -> void:
	print("aaa")
	# 启动 QEMU
	startMachine(['-m','2048'])

func _process(_delta: float) -> void:
	self.sync_connection(_delta)
	var count = get_screen_count()
	#print(count)
	
	# 1. 检查并同步 Sprite 数量
	if count > 0 and screen_sprites.size() != count:
		_setup_sprites(count)
	
	# 2. 批量更新纹理
	for i in range(screen_sprites.size()):
		if get_usable(i):
			var tex = get_texture(i)
			if tex:
				screen_sprites[i].texture = tex
		else:
			pass
			

# 批量生成 Sprite 的私有函数
func _setup_sprites(count: int) -> void:
	print("Detecting %d screens, generating sprites..." % count)
	
	# 清除旧的（如果有）
	for s in screen_sprites:
		s.queue_free()
	screen_sprites.clear()
	
	# 批量生成
	var current_x_offset = 0.0 # 记录当前已经排了多宽
	
	for i in range(count):
		var new_sprite = Sprite2D.new()
		add_child(new_sprite)
		
		# 获取当前屏幕的宽高
		var w = get_width(i)
		var h = get_height(i)
		
		# 居中设置
		new_sprite.centered = true
		
		# 计算位置：
		# current_x_offset 是之前所有屏幕的总宽度
		# w / 2 是为了把当前这个 Sprite 的中心点推到正确的位置
		new_sprite.position = Vector2(current_x_offset + w / 2, h / 2)
		
		# 累加偏移量，为下一个屏幕留出空间
		current_x_offset += w + 16 # 20 是屏幕之间的间距
		#new_sprite.scale = Vector2(0.8,0.8)
		screen_sprites.append(new_sprite)
