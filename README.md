# OpenUniverse: A Brand-New Subversive Windows Desktop Application Development Technology
OpenUniverse is a disruptive open source project for Windows desktop software development, in which we provide a consistent way for Windows desktop software to support：	
### 			1. Common Web Browser Runtime(CWR), 
### 			2. Common Eclipse Runtime(CER), 
### 			3. .NET Runtime, 
			
OpenUniverse provides a **Multi-Dimensional Software Ecosystem** for Win32 Desktop Application.
## Common Web Browser Runtime(CWR or Web Browser Runtime for Application).
Common Web Browser Runtime is a **dynamic link library version of Google Chromium Project** that contains all of the Chromium Project code, with the goal of allowing Windows desktop applications to implement the **built-in modern browser model** in a unified manner, making the host desktop software a natural Web browser. The difference with browsers in the conventional sense is that applications that integrate **CWR**(Common Web Browser Runtime) have their own application object models, and because of the built-in Chromium, application developers can develop new application contents for host applications based on Web technology, making the host application a Web technology-based Software Ecosystem.

**CWR** is different from **Chromium Embbed Framework**. Compared with CEF, CWR provides all the functions of Chromium. This ensures that the smallest structural unit of desktop applications integrated with CWR is a web browser. The most obvious difference from conventional browsers is that CWR supports Application-oriented windows, and browsers usually only support browser windows. CWBR is also different from **Electron**, and the positioning between the two is completely different. CWBR is for desktop software, while Electron is committed to developing desktop software using Web technology. Electron weakens the huge resources of desktop software, which is almost two extremes of CWR's positioning.
Every application that supports CWR is a modern browser. Strictly speaking, it is an application-oriented browser, which is different from standard browsers. the fundamental difference is that CWBR supports extended DOM, allowing developers to customize their own DOM model, The fundamental difference is that CWBR supports extending DOM, allowing developers to customize their own DOM model, which endows host applications with great content ecological support. 
## Key features of CWR
- 1.	Supports all of Chromium features.
- 2.	With **the Complete Chromium Sandbox Mechanism**, this is the most direct difference from **CEF**.
- 3.	**Custom DOM**, in particular, we allow **Binary Programmable Object UI Elements** (WinForm, Usercontrol, WPF, ActiveX Control, etc.) to participate in web page development as extended DOM elements.
- 4.	Binary programmable object Web binding: 1, allows the **Renderer Process** to **listen** for events of the Browser process object(COM Object, .Net Object, Java Object and C++ Object etc.) in the Javascript session; 2, **Binary Programmable Objects**(COM Object, .Net Object, Java Object and C++ Object etc.) can send IPC messages to the Renderer process. Through the above two aspects, CWBR realizes the web page expansion development ability 
- 5.	A **Multi-Layered, Gridded UI layout Description Engine** based on DOM enables developers to implement a flexible, powerful, dynamic Web-Desktop hybrid layout based on Web technology.
## Common Eclipse Runtime(Eclipse for Application).
Common Eclipse Runtime is a **dynamic link library version of Eclipse Project**. We know that eclipse's basic structure consists of two parts, the first being an executable called **Eclipse Laucher** and the second being a Java-based component integration system. Eclipse Runtime essentially implements Eclipse Laucher as a set of dynamic link libraries, allowing a typically meaningful Windows desktop application to become Eclipse Laucher in a standard way, and integrating Eclipse Runtime applications with its own non-Java-based application model allows application developers to take full advantage of Java technology. Based on this model, Java technology is used to develop new application content for host applications, making the application system a Software Ecosystem based on Java technology.


## OpenUniverse is an open source project whose goals are:
- 1.	Provide a flexible, easy-to-describe, Internet-oriented UI layout engine for Win32 applications;
- 2.	Provide a built-in modern browser model for each Win32 application
- 3.	Let Win32 applications fully support .NET Framework (design time and runtime)
- 4.	Let Win32 applications become an Eclipse Laucher
- 5.	Let Win32 applications Support COM Based Software Ecosystem

OpenUniverse allows software developers to re-look at every Win32 desktop application with a sense of hierarchy of the universe structure, making:
1.	Thoroughly break the boundary between desktop development and Web development to adapt to the software needs of the new generation of Internet environments;
2.	To downplay the difference between desktop applications and Internet browsers, every Win32 or WinForm application can actually have a built-in modern browser model, making the application an Internet-oriented browser by default;
3.	Every WinForm or Win32 application has a sufficiently large object model and its own DOM system based on Web technology
4.	Every binary component (.NET Component or Win32 component) has Web characteristics, thus forming a dual mode;
5.	Every desktop software has the basis of Web-oriented content services. Internet browser is a special desktop application. Because it supports Web pages, its content ecology has established the real status of Internet browsers. We believe this model being restricted to the browser is a kind of restriction on productivity. One of the missions of OpenUniverse is to extend this webpage support capability to popular desktop applications, making Internet content services a kind of public rather than browser-only characteristics.

## OpenUniverse provides a Multi-Dimensional Software Ecosystem
- 1.	The first dimension is .NET Framework software ecosystem;
- 2.	The second dimension is Java software ecosystem;
- 3.	The third dimension is the Web software ecosystem
- 4.	The fourth dimension: COM-based software ecology
- 5.	The fifth dimension: C++-based software ecology
- 6.	The sixth dimension: Software ecology for Office documents

## From Galileo to Herschel to Hubble, every breakthrough in cosmic observation is a cognitive subversionFrom Galileo to Herschel to Hubble, every breakthrough in cosmic observation is a cognitive subversion
Galileo invented the first astronomical telescope in the true sense of mankind, allowing mankind to have a glimpse of the solar system, Herschel expanded the field of vision of mankind to the Milky Way. The emergence of Hubble enabled human beings to have a glimpse of the universe. The influence of Hubble is so profound that even Einstein had to modify the key links of his theory of relativity because of the discovery of Hubble. Up to now, our understanding of the universe is a cosmic system with galaxy clusters and more and larger celestial structures as its structural members. All this benefits from NASA's Hubble. Without Hubble telescope, we can hardly imagine all this.
In the era of observing the sky with the naked eye, many links can only be explained by imagination, so people have created a lot of "myths", which is almost the same in China and foreign countries. Due to the limitations of the naked eye, people can only see a limited universe. At today's scale, the universe under the naked eye is very, very "small". We know that in some celestial images taken by Hubble, the size of a pixel in the image is estimated to be 5 light years, which is much larger than the scale of our solar system. In many cases, everything we see may be very different from the real scene.
Going back to the WinForm program, many people will say that our software has only a limited number of WinForms. What does this have to do with the astronomical observations you mentioned? History tells us that if we don’t observe the sky, the earth is a cage, and we will be imprisoned forever on this small planet. Similarly, for WinForm application development, if we cannot find inspiration from observations of the universe, then our world will be another cage, which will greatly limit our creativity.
From the perspective of OpenUniverse, every WinForm application is a universe in a sense. Or they all have their own cosmic architecture. It sounds ridiculous, how can an ordinary WinForm application have its own cosmic architecture?

## WinForm applications are a universe in a sense
If a WinForm application has a hierarchy similar to the universe, we need to answer several key questions:
1.	Where is the WinForm object in this universe?
2.	Are there galaxies in this universe?
3.	Where are the various stars in this universe?
4.	How do we view this universe?

We know that the most basic programmable elements of WinForm development include .Net control, WinForm, WPF, These objects belong to the .Net framework. For a specific application, a specific .Net project can only contain a limited number of elements we just mentioned. Each project has its own boundary, just like a small cage, in which its own elements are placed. Such a project is either an independent executable file or a component library. This has been the state since the birth of the .Net framework.
From a mathematical point of view, modern mathematics is accustomed to putting the same kind of things together to form a brand new set, and then define new rules on this set, and the unified effect on the elements of the set, for example, Vector Space is a typical example. Group is also an example. The aggregation of similar objects in mathematics to form a new mathematical structure is an obvious law of modern mathematics. If we look at .NET WinForm development in a similar way, we try to imagine how big the collection will be if all the .NET UI objects are aggregated in a new collection?
Obviously, this is a very large collection of infinitely many elements. We divide the members of this collection into two categories. The first category is that the elements belong to a DLL library, and the second category is that the elements belong to exe. the structure of this set is similar to a mathematical structure: fiber bundle. We will give a systematic introduction to this kind of structure in the following introduction.For a single WinForm application, a more obvious "fiber" refers to the program itself as the fiber base point, on which a huge fiber "grows": the collection of. Net UI components existing in the DLL library. 
![Fiber Bundle](https://raw.githubusercontent.com/TangramDev/UniverseWeb/master/image/fiberbundle.png "Fiber Bundle")
From an abstract point of view, the collection of UI components contained in the DLL library is large enough to be said to be "all inclusive". We believe that there is a "universe", a new "cosmic space" formed by the members contained in the application executable itself, as well as the members within the entire giant fiber.
According to the technical specifications provided by OpenUniverse, every WinForm application is the WinForm executable program itself plus a huge "component space". This component space at least contains "all UI components contained in the publicly accessible DLL library", which is a very large component collection.
## Everything starts with the Dock property
As we know, each. Net UI object (WinForm, control, WPF control) has a dock property, which has six values. Therefore, we divide the. Net UI objects into two categories according to the dock property. The first category is the dock property with values of "left, right, top, bottom", this kind of control can only surround the edge of another UI object. The other type of dock property value is "fill, none". The second type of object is worthy of our attention. In fact, in the. Net design logic, the second type of object will be surrounded by other objects. In our universe, the second type of object is very important. These objects are equivalent to the galactic nuclei in the universe. It sounds like this It's incredible that the galactic nucleus should be surrounded by a lot of stars, but we don't see it.

## Quasars and Dock properties
What you see may be completely different from the real one, this is how the discovery of quasars is. Quasars are one of the most important celestial bodies in modern astronomy. When they were first discovered, quasars were a kind of strong radio source. By chance, people found the optical counterpart of this kind of radio source. It seems that it is a star with a "round surface", which is extremely bright. Due to the huge red shift, according to Hubble's law, such objects are far away from our speed, The speed of a quasar with a diameter of 4 light-years can reach 70% of the speed of light far away from us. It is very hard to imagine. People found such objects and did not know what they were for a long time. From the discovery in the 1960s, until the separation of the "spiral arms" around the quasars by Hubble in 2011, People realize that quasars are the “nuclei of a class of violently active galaxies. As you can see from their names, people initially thought that these celestial bodies seemed to be similar to “stars”, but much larger than stars.
      Without Hubble, it is very difficult to associate quasars with galactic nuclei. In other words, Hubble allows us to see a large number of stars surrounding quasars. Will similar phenomena occur in the development of WinForm? If what we say here is true, then WinForm application development will be very different from the mode we see at present. Similar to cosmic observation, we need something similar to Hubble. All this, openuniverse is ready for us.
Generally speaking, every specific WinForm contains at least one control, and its Dock property is either "Fill" or "None". If WinForm can be interpreted as a kind of celestial structure in the universe, then this kind of celestial structure is the "galaxy cluster" in the modern universe, and the core of each galaxy in these galaxy clusters corresponds to the .NET UI elements whose Dock attribute value is "Fill" or "None". We give a solution in OpenUniverse, so that all kinds of objects surrounding such controls can be presented.
In the OpenUniverse project, we implemented a very important core object, which we named "Hubble". This object is aimed at the. Net controls and WPF objects whose dock property value is "fill" or "None".
## Virtual Layout Engine for Win32
	我们需要几个基本概念，第一个概念是“网格”
The grid is a window matrix with m rows and n columns, and a grid with one row and n columns can be a “tab window” with n tabs in the usual sense.
1、网格是可嵌套的，这一点意味着，每一个网格之中的“格子”，可以是另外一个或者一组“网格，如果是一组，那么运行时只有一个是可见的。
2、网格是可填充的，就是说，一个格子，可以用一个具体的对象填充
3、网格是可描述的
4、网格是可编程的

## If you own Hubble, you will open up a whole new universe
Unfortunately, Hubble belongs to NASA, so the entire astronomy community must obey NASA's specifications before using Hubble. Hubble has created NASA's monopoly. In the WinForm world, each application has its own universe system. Through OpenUniverse, we equip each software with Hubble that belongs to the application itself, where developers can use Hubble to observe everything, they are interested in
