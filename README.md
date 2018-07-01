# :construction: Work in Progress :construction:

# Neural scene representation and rendering

[https://deepmind.com/blog/neural-scene-representation-and-rendering/](https://deepmind.com/blog/neural-scene-representation-and-rendering/)

**Todo**

- [x] OpenGL Renderer
- [x] Implement GQN
- [ ] Implement training loop
- [ ] Debug

# Requirements

- pybind11
- Python 3
- OpenGL 4
- GLFW 3
- Ubuntu
- C++14 (gcc-6)
- Chainer 4

# Installation

**GLFW**

```
sudo apt install libglfw3-dev
```

**pybind11**

```
conda install -c conda-forge pybind11
```

**Renderer**

```
cd three
make
```

**imgplot**

```
cd imgplot
make
```

**Chainer**

```
conda install -c conda-forge chainer
conda install -c conda-forge cupy
conda install -c conda-forge h5py
```

# Dataset

## Rooms with multiple objects

## Shepard-Metzler objects

![Dataset](https://qiita-image-store.s3.amazonaws.com/0/109322/a2777cad-36b6-4c28-b8e8-a6bca16af03a.gif)

![Objects](https://qiita-image-store.s3.amazonaws.com/0/109322/ca32401b-ad69-cbc0-572b-8f9b8e0b62b4.gif)

![Environment](https://qiita-image-store.s3.amazonaws.com/0/109322/bf87c553-02ce-9568-0cc5-32b2d704d24b.gif)


# Training
# Experimental Results
