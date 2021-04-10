# SoftRasterizer

该软光栅渲染器受  [ssloy](https://github.com/ssloy)/**[tinyrenderer](https://github.com/ssloy/tinyrenderer)** 启发与其教程的学习。为了更好地理解光栅化、实时渲染机器的知识，再次记录自己在开发软光栅渲染器中实现的效果以及遇到的问题。

 This Renderer is a software rasterizer written by C++ .The proposal for writing this is for better understanding computer graphics concept and knowledge.

### 学习过程中的资料

- 《C++ prime》
- GAMES101、GAMES202

- https://learnopengl.com/

- 《fundamentals of computer graphics》

- 《real-time redering  》

- 《RTR3提炼总结》



## Features

- Camera moving
- Light moving
- orthographic projection
- perspective projection
- zbuffer
- Gouraud render
- Phong render
- Normal mapping
- Tangent space normal mapping
- Shadow mapping

## TODO List

- [ ] 添加多个物体
- [ ] 透视矫正
- [ ] MSAA
- [ ] SSAA
- [ ] PCSS
- [ ] Ambient Occlusion

## ScreenShots

### Basic model  



![image-20210326160630849](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/texture.png?raw=true)

  ![diabolo_show](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/diabolo_show.png?raw=true)

### Rendering  



![show1](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/show1.png?raw=true)

![diabolo_shadow_show](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/diabolo_shadow_show.png?raw=true)

![man_shadow_show](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/man_shadow_show.png?raw=true)









## Troubleshooting  


### 1.渲染的图像有横纵裂纹  

![image-20210405152927370](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/1-1.png?raw=true)

![1-2](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/1-2.png?raw=true)



- 问题：遍历屏幕像素时用了浮点数float
- 解决方案： 应该将插值得到的屏幕位置用整型表示，同时将插值的Z额外用浮点保存。  



### 2. 渲染的颜色错误  



![](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/2-1.png?raw=true)



- TGAColor的初始化是BGR，不是RGB。所以color[2] 和 color[0]反了导致颜色呈蓝色。



### 3. 图像变成了左手系的原因  


![image-20210405153810153](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/3-1.png?raw=true)

![image-20210405183213062](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/3-2.png?raw=true)

- 正面的法线方向是朝着Z轴的，和朝着-z方向的光照 所成夹角应该是负的。 

### 4.纹理映射错误  



![image-20210405180353833](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/4-1.png?raw=true)



- 所有的纹理是混乱的，而不是上下颠倒或者左右翻转。那么就考虑是在数据读取阶段出了问题，最后查出使用了错误的三角形索引

### 5.透视投影错误  



![image-20210405184750541](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/5-1.png?raw=true)



- 齐次坐标转换时，直接用了(x,y,z) . 没有除以最后一项。 

- 透视投影后，齐次项的表示有可能是（x,y,z, 2) 这种形式 ， 整个坐标要除以最后的齐次项才正确 ，如：（x/2 , y/2 , z/2 , 1) 



### 6.Bling-Phong模型 （用ks 还是 I来描述颜色？）

![preview](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/6-1.jpg?raw=true)





用系数值ka、ks、kd表示贴图读取的值，    I以及r进行另外设置。

（当光源距离固定时、光源的光照强度越强，则物体表面被照亮的强度就应该越高。）

kd , ks--> TGAColor （0-255）

ka --> (0 - 1)  Ia = Vec3f(155,155,0)



### 7. 高光贴图呈紫色的原因

![image-20210408161414753](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/7-1.png?raw=true)

高光贴图只有r有值，我一开始以为是rgb，分了三个ks，得出来的结果偏紫，后来才发现读的图片像素颜色是(r,0,0)

在高光贴图时使用color[0]付给TGAColor的三个通道即可。



### 8. bling-phong模型中 h 和 normal夹角的正负

h为光照方向与相机方向的夹角。

在程序设置是，光照向量方向为:光源-->point 。相机向量方向为 相机->point.

那么得到的h与normal的夹角为钝角，应对h取反。  



###  9.Shadow mapping 中，一直只索引到shadow_buffer的一个点。

![image-20210409144347100](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/91.png?raw=true)



### 10.Shadow mapping 中 light使用正交投影和透视投影的区别

- view坐标下的物体z值 ，会在透视投影中压向视锥体的远平面（GAMES101扩展部分）

- 而正交投影只是简单把物体映射到[-1,1]^3 之间，不会影响相对位置关系。



### 11.Shadow mapping中 摩尔纹的问题

![image-20210409211406998](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/11-1.png?raw=true)![image-20210410110113770](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/11-2.png?raw=true)

### 12.正交投影后，深度值超过了[-1,1]

![image-20210410091243468](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/12-1.png?raw=true)


  阴影图的索引和screen coords的索引偏差很大。
![image-20210410143526019](D:\project\zjx\GameDevoplement\SoftRasterizer\output\12-2.png)



- 9、11、12的问题都是源自用float shadow_buffer[width * height] 数组的精度问题。
- 解决方法:替换成TGAImage的纹理图像，对读取进来的数值映射回[-1,1]之间即可。



### 13.对切线空间的理解

![](https://github.com/Waterbearbear/SoftRasterizer/blob/main/output/13-1.png?raw=true)