> *Powered by [李晟赫](https://github.com/btkrc) | [Zekalryn's Blog](https://zkryn.com)*

#  说明 (Dependencies)

1. **Qt Framework**: 基于 Qt (需自行配置 VS 或 Qt Creator 环境) 动态链接编译。
2. **大漠插件 (dm.dll)**: 由于版权及安全原因，本仓库**不提供**该 DLL 文件。请自行前往大漠插件官网获取。
   * 请使用管理员权限注册组件：`regsvr32 dm.dll`
   * 将 `dm.dll` 放置在项目根目录。
3.  `config.save` 重命名为 `config.ini`，并填入你自己的配置信息。
4. **环境凭证**：
   * 本程序依赖特定的Cookies文件（即 `config.ini` 中的 `CookiesPath` 参数）来处理网络鉴权。
   * 请自行通过合法途径获取目标客户端的鉴权数据，并在根目录下生成对应的凭证文件。

# 配置文件

**按照如下格式添加config.ini**

[DevOption]  
HOSPath=游戏执行程序路径  
szExeFile=在任务管理器中的名字  
CookiesPath=Cookies文件路径    
CurrentIndex=当前帐号下标  

[paintSettings]  
smallPaintLeft=295  
smallPaintTop=145  
smallPaintRight=935  
smallPaintBottom=660  
largePaintLeft=235  
largePaintTop=114  
largePaintRight=995  
largePaintBottom=725  

[waitSettings]  
waitLeft=1400  
waitTop=-610  
waitRight=2440  
waitBottom=149  
RGBPointerX1=1919  
RGBPointerY1=10  
RGB1=5872381  
RGBPointerX2=1905  
RGBPointerY2=72  
RGB2=12766718  

[gameSettings]  
gameLeft=-8  
gameTop=210  
gameRight=1237  
gameBottom=1047  
limitLeft=0 
limitRight=1227  
limitTop=241  
limitBottom=1035  


[logins]  
1\username=  
1\password=  
2\username=  
2\password=  
size=
