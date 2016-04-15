/*! \mainpage ATOM3D���������ҳ
 
 	������ATOM3D�����ĵ�
	- \subpage intro
	- \subpage changes
*/

/*! \page intro ���
	ATOM3D��һ��ȫ����ʵʱ��ά��Ⱦ����
*/

/*! \page changes ATOM3D�޸ļ�¼

<h4>2012-3-9 �汾 0.6.4</h4>
<ul>
	<li>��������Ч�ظ����ص�BUG</li>
	<li>�������޷���ȾGLARE��BUG</li>
	<li>�����˳����䰵��BUG</li>
	<li>�����˱༭������V��BUG</li>
	<li>������͸��������ʾ��BUG</li>
</ul>

<h4>2012-3-1 �汾 0.6.3</h4>
<ul>
	<li>�����˶��߳������ʱ�����ATOM_ContentStream::waitForInterfaceDone�������������BUG</li>
</ul>

<h4>2012-2-29 �汾 0.6.2</h4>
<ul>
	<li>
		������Уɫ�ĺ���Ч�������������˱༭���Ĺ�������<br>
		�ڴ�����ʹ����ATOM_ColorGradingEffect��ʵ��Уɫ<br>
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
	<li>������SSAOЧ��ѡ��(ģ���㷨����Ľ�)</li>
		\code
			void enableSSAO (bool enable)
			{
				ATOM_RenderSettings::enableSSAO (enable);
			}
		\endcode
	<li>�����˶�sRGBɫ�ʿռ��֧��</li>
	<li>�޸���ATOM_Texture�ӿ���֧�������ͼ������</li>
	<li>�Ľ���ATOM_studio�༭���Ĳ˵���ʾ����</li>
	<li>������GUI�ؼ��رհ�ť�޷���ȷ��Ӧ��BUG</li>
</ul>

<h4>2012-2-13 �汾 0.6.1</h4>
<ul>
	<li>�Ż��˰˲���������Ч��</li>
	<li>ɾ����һЩ�����Ĵ���</li>
	<li>�������ڵ�͸��Ч����BUG</li>
	<li>�����˵�������ӰʱCPU��Ƥ�������ε�BUG</li>
</ul>

<h4>2012-2-10 �汾 0.6.0</h4>
<ul>
	<li>�Ľ�����׶����Ч��</li>
	<li>�����˹���������ģ�ͱ��ε�����</li>
</ul>

<h4>2012-2-8 �汾 0.5.9</h4>
<ul>
	<li>��mesh�Ĺ�����������75����������Զ�ʹ��GPU������Ƥ��������</li>
	<li>�Ż���GUI�ı����ٶȡ�</li>
	<li>�����˶�ջ���١�</li>
	<li>
		tinyxml�ڲ����ڴ������ԭ����new/delete��ΪATOM_NEW/ATOM_DELETE,�ͻ�����ش���Ҳ�����޸ģ��������������������磺<br>
		\code
			pElem = new TiXmlElement("float");
		\endcode
		��Ҫ��Ϊ��<br>
		\code
			pElem = ATOM_NEW(TiXmlElement, "float");
		\endcode
	</li>
</ul>

<h4>2012-1-5 �汾 0.5.8</h4>
<ul>
	<li>�޸��˺���ATOM_InitializeGfx��ȥ���˵�һ��DisplayMode������Ҫ����ȫ��ģʽ��primaryWindowDesc�еĳ�Աfullscreen��Ϊtrue���ɡ�</li>
	<li>�����˺���ATOM_ToggleFullScreen�����ô˷������ڴ��ں�ȫ��ģʽ���л���</li>
</ul>

<h4>2011-12-29 �汾 0.5.7</h4>
<ul>
	<li>�ڴ����а��ȼ�Ctrl+Shift+`���Ժ����ű����ڡ��ڽű������п�����lua�ű���</li>
</ul>

<h4>2011-12-23 �汾 0.5.5</h4>
<ul>
	<li>�����˲�����Ŀ��δ�õ�����Ⱦ�����������ȾЧ�ʡ�</li>
	<li>
		Ĭ�ϵ�ˮ����Ⱦ��������Ϊ�У����޷��䡣<br>
		��ͨ��ATOM_RenderSettings::setWaterQuality()�����޸�ˮ����Ⱦ������<br>
	</li>
	<li>
		��ӽӿ�ATOM_RenderSettings::enableCloudCover(bool enable)��<br>
		ʹ�ô˽ӿ��������Ƿ���Ⱦ�Ʋ㡣��ֹ��Ⱦ�Ʋ����Ч�������нϴ���������ڸ��ӽ���Ŀ����򿪴��<br>
	</li>
	<li>
		��ӽӿ�ATOM_RenderSettings::setAtmosphereQuality()��<br>
		ʹ�ô˽ӿ������ƴ�����Ч����Ⱦ������<br>
	</li>
</ul>

<h4>2011-12-19 �汾 0.5.0</h4>
<ul>
	<li>
		ATOM_Application�����������´����쳣���麯���ӿڣ��ɱ�������ʵ���Զ����쳣����<br>
		ATOM_Application::handleWin32Exception<br>
		ATOM_Application::handleCppException<br>
		ATOM_Application::enableErrorPopup<br>
		ATOM_Application::isErrorPopupEnabled<br>
	</li>
	<li>
		�����˺�ATOM_STACK_TRACE��ʵ�ֶ�ջ���ٹ��ܡ�<br>
		��������˶�ջ���٣��������쳣�������ٶ�ջ���ᱻ�������־<br>
	</li>
	<li>
		֧�ֽػ�VC�е�abnormal termination���󣬵������˴���ʱ���׳�C++�쳣��
	</li>
	<li>
		֧�ֲ���ֱ��ͨ��OutputDebugString��������ĵ�����Ϣ<br>
		��������ڵ����������еĻ���ʹ��OutputDebugString��������ĵ�����Ϣ�ᱻת������־���˹�����Ҫ����֧��D3D�������п⡣<br>
	</li>
	<li>
		������D3D���Թ��ܡ�<br>
		Ҫ����D3D���Թ��ܣ���Ҫ�ڳ�������Ŀ¼����D3D9D.DLL��D3DX9D_33.DLL�����ļ���ͨ�����������в���--debugd3d���Կ���D3D���ԣ�<br>
		����D3D���Ժ�D3D������ϢҲ���������־��<br>
		����������ATOM3D_ENABLE_D3D_DEBUG������ΪYESʱ����������ʱ�����м���D3D�������п⡣���δ�ҵ��������п�����ڻ�������ATOM3D_D3D_DEBUG_RUNTIME_URLָ���ĵ�ַ����D3D���Կ⵽��ǰĿ¼�����ء����ʧ�������D3D���а����п⡣<br>
	</li>
	<li>
		�������ڴ�й¶�ĵ��Թ��ܡ�<br>
		����ͬ����Դ�ļ�λ�÷����ۼƳ���1000�ε�malloc����ȥ��Ӧ��free�Ĵ��������ᱻ��¼Ϊһ�����ܵ��ڴ�й¶�����log�
		�Ժ�ÿ������100��malloc���þͻ��ٴ��������Ϣ�������ʽΪ "*** Possible memory leak detected"��������Ÿô��ڴ����
		�������õ�StackTrace���ٽ�ֵ1000��ͨ����������ķ������á�<br>
		ATOM_SetAllocCounterAlert<br>
		����һ�ֵ��Է�������������в���--debugmemѡ�
		�ڼ�⵽ʹ�õ��ڴ�����ض��ٽ�ֵ��ʱ�����ATOM_DumpMemoryState()�����������ǰ��������ʹ�õ��ڴ��嵥���ļ���<br>
		����������ATOM3D_ENABLE_MEM_DEBUG������ΪYESʱ���������ڴ����ģʽ������<br>
		ע�����Ϲ��ܽ���ʹ��ATOM_MALLOCϵ���ڴ���亯����������ڴ���Ч��
	</li>
	<li>
		�޸�BUG:������ĳ��ԭ����Ⱦѭ��ͣ�ٱȽϾõ�ʱ��(������������)֮���ٴν�����Ⱦѭ��������ϵͳ�����ͣ�ٵ�ʱ�����������������ӣ������ڴ漤�����������ʱ��ֹͣ��Ӧ����
	</li>
	<li>
		������ATOM_net����֧��internet����(FTP,HTTP)���˹���Ŀǰ����D3D���ԡ�����ʱ��Ҫ����ATOM_net.lib��
	</li>
</ul>

*/
