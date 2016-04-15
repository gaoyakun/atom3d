/*! \mainpage ATOM3D引擎帮助主页
 
 	这里是ATOM3D帮助文档
	- \subpage intro
	- \subpage changes
*/

/*! \page intro 简介
	ATOM3D是一款全功能实时三维渲染引擎
*/

/*! \page changes ATOM3D修改记录

<h4>2012-3-9 版本 0.6.4</h4>
<ul>
	<li>修正了特效重复加载的BUG</li>
	<li>修正了无法渲染GLARE的BUG</li>
	<li>修正了场景变暗的BUG</li>
	<li>修正了编辑框输入V的BUG</li>
	<li>修正了透明物体显示的BUG</li>
</ul>

<h4>2012-3-1 版本 0.6.3</h4>
<ul>
	<li>修正了多线程载入的时候调用ATOM_ContentStream::waitForInterfaceDone可能造成死锁的BUG</li>
</ul>

<h4>2012-2-29 版本 0.6.2</h4>
<ul>
	<li>
		增加了校色的后处理效果，并集成在了编辑器的工作流中<br>
		在代码中使用类ATOM_ColorGradingEffect来实现校色<br>
		\code
			void enableColorGradingEffect (ATOM_RenderDevice *device, ATOM_Scene *scene, const char *lutTextureFileName, bool linearFilter)
			{
				ATOM_AUTOREF(ATOM_Texture) lutTexture = device->createTextureResource(lutTextureFileName);
				ATOM_AUTOPTR(ATOM_ColorGradingEffect) effect = ATOM_NEW(ATOM_ColorGradingEffect);
				effect->setLUT (lutTexture.get());
				effect->enableLinearFilter (linearFilter);
				scene->setPostEffect (effect.get());
			}
		\endcode
	</li>
	<li>增加了SSAO效果选项(模糊算法尚需改进)</li>
		\code
			void enableSSAO (bool enable)
			{
				ATOM_RenderSettings::enableSSAO (enable);
			}
		\endcode
	<li>增加了对sRGB色彩空间的支持</li>
	<li>修改了ATOM_Texture接口以支持体积贴图的载入</li>
	<li>改进了ATOM_studio编辑器的菜单显示界面</li>
	<li>修正了GUI控件关闭按钮无法正确响应的BUG</li>
</ul>

<h4>2012-2-13 版本 0.6.1</h4>
<ul>
	<li>优化了八叉树遍历的效率</li>
	<li>删除了一些废弃的代码</li>
	<li>修正了遮挡透视效果的BUG</li>
	<li>修正了当开启阴影时CPU蒙皮运算两次的BUG</li>
</ul>

<h4>2012-2-10 版本 0.6.0</h4>
<ul>
	<li>改进了视锥剪裁效率</li>
	<li>修正了骨骼动画中模型变形的问题</li>
</ul>

<h4>2012-2-8 版本 0.5.9</h4>
<ul>
	<li>当mesh的骨骼数量少于75个的情况下自动使用GPU加速蒙皮动画；。</li>
	<li>优化了GUI的遍历速度。</li>
	<li>禁用了堆栈跟踪。</li>
	<li>
		tinyxml内部的内存分配由原来的new/delete改为ATOM_NEW/ATOM_DELETE,客户端相关代码也必须修改，否则会引起崩溃！！例如：<br>
		\code
			pElem = new TiXmlElement("float");
		\endcode
		需要改为：<br>
		\code
			pElem = ATOM_NEW(TiXmlElement, "float");
		\endcode
	</li>
</ul>

<h4>2012-1-5 版本 0.5.8</h4>
<ul>
	<li>修改了函数ATOM_InitializeGfx，去掉了第一个DisplayMode参数，要设置全屏模式将primaryWindowDesc中的成员fullscreen设为true即可。</li>
	<li>增加了函数ATOM_ToggleFullScreen，调用此方法以在窗口和全屏模式间切换。</li>
</ul>

<h4>2011-12-29 版本 0.5.7</h4>
<ul>
	<li>在窗口中按热键Ctrl+Shift+`可以呼出脚本窗口。在脚本窗口中可运行lua脚本。</li>
</ul>

<h4>2011-12-23 版本 0.5.5</h4>
<ul>
	<li>屏蔽了部分项目中未用到的渲染功能以提高渲染效率。</li>
	<li>
		默认的水体渲染质量设置为中，即无反射。<br>
		可通过ATOM_RenderSettings::setWaterQuality()方法修改水体渲染质量。<br>
	</li>
	<li>
		添加接口ATOM_RenderSettings::enableCloudCover(bool enable)。<br>
		使用此接口来控制是否渲染云层。禁止渲染云层对于效率提升有较大帮助，对于俯视角项目建议打开此项。<br>
	</li>
	<li>
		添加接口ATOM_RenderSettings::setAtmosphereQuality()。<br>
		使用此接口来控制大气雾效的渲染质量。<br>
	</li>
</ul>

<h4>2011-12-19 版本 0.5.0</h4>
<ul>
	<li>
		ATOM_Application类增加了以下处理异常的虚函数接口，可被派生类实现自定义异常处理<br>
		ATOM_Application::handleWin32Exception<br>
		ATOM_Application::handleCppException<br>
		ATOM_Application::enableErrorPopup<br>
		ATOM_Application::isErrorPopupEnabled<br>
	</li>
	<li>
		增加了宏ATOM_STACK_TRACE以实现堆栈跟踪功能。<br>
		如果启用了堆栈跟踪，当发生异常错误后跟踪堆栈将会被输出到日志<br>
	</li>
	<li>
		支持截获VC中的abnormal termination错误，当发生此错误时会抛出C++异常。
	</li>
	<li>
		支持捕获直接通过OutputDebugString方法输出的调试信息<br>
		如果不是在调试器内运行的话，使用OutputDebugString方法输出的调试信息会被转发到日志。此功能主要用于支持D3D调试运行库。<br>
	</li>
	<li>
		增加了D3D调试功能。<br>
		要启用D3D调试功能，需要在程序运行目录发布D3D9D.DLL和D3DX9D_33.DLL两个文件。通过增加命令行参数--debugd3d可以开启D3D调试，<br>
		开启D3D调试后D3D调试信息也会输出到日志。<br>
		当环境变量ATOM3D_ENABLE_D3D_DEBUG被设置为YES时，程序启动时会自行加载D3D调试运行库。如果未找到调试运行库则会在环境变量ATOM3D_D3D_DEBUG_RUNTIME_URL指定的地址下载D3D调试库到当前目录并加载。如果失败则加载D3D发行版运行库。<br>
	</li>
	<li>
		增加了内存泄露的调试功能。<br>
		当在同样的源文件位置发生累计超过1000次的malloc（消去对应的free的次数）将会被记录为一个可能的内存泄露输出到log里。
		以后每多增加100次malloc调用就会再次输出该信息。输出格式为 "*** Possible memory leak detected"，后面跟着该次内存分配
		函数调用的StackTrace。临界值1000可通过调用下面的方法设置。<br>
		ATOM_SetAllocCounterAlert<br>
		另外一种调试方法：添加命令行参数--debugmem选项，
		在检测到使用的内存大于特定临界值的时候调用ATOM_DumpMemoryState()方法可输出当前所有正在使用的内存清单到文件。<br>
		当环境变量ATOM3D_ENABLE_MEM_DEBUG被设置为YES时，程序以内存调试模式启动。<br>
		注：以上功能仅对使用ATOM_MALLOC系列内存分配函数所分配的内存有效。
	</li>
	<li>
		修复BUG:当出于某种原因渲染循环停顿比较久的时间(例如锁定电脑)之后，再次进入渲染循环，粒子系统会根据停顿的时间间隔产生大量的粒子，导致内存激增，计算机暂时性停止响应。。
	</li>
	<li>
		增加了ATOM_net库以支持internet下载(FTP,HTTP)，此功能目前用于D3D调试。编译时需要链接ATOM_net.lib。
	</li>
</ul>

*/
