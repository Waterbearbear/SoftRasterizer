# HandCraft Rasterizer Renderer

This project was inspired from [ssloy](https://github.com/ssloy)/**[tinyrenderer](https://github.com/ssloy/tinyrenderer)**

 This Renderer is a software rasterizer written by C++ .The proposal for writing this is for better understanding computer graphics concept and knowledge.

## ScreenShots

![image-20210326160630849](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/texture.png?raw=true)

![image-20210326160647373](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/texture1.png?raw=true)





## Features

- Texture mapping
- z-buffer
- perspective projection
- GouraudShader
- Bling-Phong
- Normal mapping
- Tangent normal mapping



## Debug日志  


### 1.渲染的图像有横纵裂纹  


![image-20210405152927370](C:\Users\asus\AppData\Roaming\Typora\typora-user-images\image-20210405152927370.png)

- 遍历屏幕像素时用了浮点数，应该将插值得到的屏幕位置用整型表示，这样就不会出错，同时将插值的Z额外用浮点保存。  



### 2. 渲染的颜色错误  



![QQ图片20210405153445](D:\project\zjx\GameDevoplement\SoftRasterizer\output\QQ图片20210405153445.png)



- TGAColor的初始化是BGR，不是RGB。所以color[2] 和 color[0]反了。



### 3. 图像变成了左手系的原因  


![image-20210405153810153](C:\Users\asus\AppData\Roaming\Typora\typora-user-images\image-20210405153810153.png)

![image-20210405183213062](C:\Users\asus\AppData\Roaming\Typora\typora-user-images\image-20210405183213062.png)

- 正面的法线方向是朝着Z轴的，和朝着-z方向的光照 所成夹角应该是负的。 



### 4.纹理映射错误  



![image-20210405180353833](C:\Users\asus\AppData\Roaming\Typora\typora-user-images\image-20210405180353833.png)









### 5.透视投影错误  



![image-20210405184750541](C:\Users\asus\AppData\Roaming\Typora\typora-user-images\image-20210405184750541.png)



齐次坐标转换时，直接用了(x,y,z) . 没有除以最后一项。 

透视投影后，齐次项的表示有可能是（x,y,z, 2) 这种形式 ， 整个坐标要除以最后的齐次项才正确 ，如：（x/2 , y/2 , z/2 , 1) 



### 6.Bling-Phong模型 （用ks 还是 I来描述颜色？）





#### 

![preview](https://pic3.zhimg.com/v2-4592bb9c2faa811358c0de63516538ae_r.jpg)



![image-20210407140117214](C:\Users\asus\AppData\Roaming\Typora\typora-user-images\image-20210407140117214.png)

spec： 

按照这个表示方法，那么就是c * 

明显是用系数值ka、ks、kd.      diffuse和specular和I是代表的光照强度。

kd , ks--> TGAColor （0-255）

ka --> (0 - 1)  Ia = Vec3f(155,155,0)



### 7. 高光贴图呈紫色的原因

![image-20210408161414753](C:\Users\asus\AppData\Roaming\Typora\typora-user-images\image-20210408161414753.png)

高光贴图只有r有值，我一开始以为是rgb，分了三个ks，得出来的结果偏紫，后来才发现读的图片像素颜色是(r,0,0)

在高光贴图时使用color[0]付给TGAColor的三个通道即可。



### 8. bling-phong模型中 h 和 normal夹角的正负





###  9.Shadow mapping 中，一直只索引到shadow_buffer的一个点。

![image-20210409144347100](C:\Users\asus\AppData\Roaming\Typora\typora-user-images\image-20210409144347100.png)



### 10.Shadow mapping 中 light使用正交投影和透视投影的区别



### 11.Shadow mapping中 摩尔纹的问题

![image-20210409211406998](C:\Users\asus\AppData\Roaming\Typora\typora-user-images\image-20210409211406998.png)