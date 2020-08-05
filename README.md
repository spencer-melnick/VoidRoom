# GPU Ocean Wave Simulation

![Animated Ocean Wave Simulation](https://github.com/spencer-melnick/spencer-melnick/raw/master/images/ocean_sim.gif)

## What is it?

This is an implementation in Unreal Engine 4 of a technique for realtime wave simulation based upon [Jerry Tessendorf's 2001 Paper](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.161.9102&rep=rep1&type=pdf), and famously used in the film Titanic (meaning this technique is older than I am!)

Of course, while the technique has not changed fundamentally since 1997, recent advances in computer hardware allow the simulation, which once had to be performed offline on render farms, in realtime.

This implementation is largely based on a [2017 paper by Fynn-Jorin Flügge](https://tore.tuhh.de/bitstream/11420/1439/1/GPGPU_FFT_Ocean_Simulation.pdf) with my own modifications to how the butterfly operations are precomputed and stored, leading to lower memory usage and preventing duplicate operations (the original texture method described by Flügge results in butterfly operations being performed for each cell on the FFT, while butterfly operations only need to be performed once per pair of operands).

## Unreal Engine Compute Shader

Developing the computer shaders in Unreal Engine took a substantial amount of time, as much of the global shader system has changed in recent releases, making old documentation and examples no longer valid. It took me quite a while of poring through engine, reading powerpoint presentations, and debugging to finally get the shaders to function. A particular challenge was figuring out how to send data from the CPU to the buffers created by Unreal's Realtime Hardware Interface (RHI) layer. In particular, I am still not able to send data directly from the CPU to buffers allocated by Unreal's new Render Dependency Graph (RDG), but this implementation does use RDG for intermediate passes.

## How does it work?

The technique (from my perspective as an engineering student) is really quite clever! Instead of relying on large numbers of Gerstner waves which has an increasing performance cost with each additional wave, it uses the ability of an inverse fourier transform to scale much better with additional frequency resolution. Of course, it's highly unlikely that all 512x512 samples of your frequency data will have a notable effect (while rendering a 512x512 displacement texture), but it does allow for a very high level of detail at lower the cost than a simple summed Gerstner wave approach.

This is of course a very basic explanation, and may not be entirely accurate. If you want to learn more, you should read the above paper by Jerry Tessendorf.

## How to use it

Using this code simply requires placing an actor into your level that can control the parameters and direct the output of the shader passes. Specifically, this actor writes the simulation results (X, Y, and Z displacement, turbidity, and gradient) to `URenderTexture2D`s that can be used directly in any standard Unreal material.

## What's the deal with this repository?

If you might have noticed, this project is a non-master branch of another project. I had originally branched off from that project, hoping to add this ocean simulation to the main project once the simulation was completed. Ultimately though, the main project ended up going in a different direction, and the ocean simulation is likely to be replaced with the new water tech coming in Unreal Engine 4.26.

Still, I like to keep this around to show an example of a full FFT based simulation (to my knowledge, the new tech uses Gerster waves, as they allow for better integration with gameplay, since it's not practical to use GPU computed FFT simulation results on collision data, particularly in multiplayer games) and how to implement custom compute shaders in the latest version of Unreal Engine.
