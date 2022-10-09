# Welcome to PRL4AirSim

**Currently updating the documentation, hopefully will be done over the next couple of days!**

This project builds on top of [AirSim](https://github.com/microsoft/AirSim) to provide efficient parallel training for Reinforcement Learning controllers.  Check out our 3 minute video bellow which explains our approach.


<p float="left">
  <img src="PRL4AirSim/images/ParallelRLDrones.gif" width="40%" />
  &emsp;&emsp;
  <a href="https://www.youtube.com/watch?v=kAWbEUUT8bw">
    <img src="PRL4AirSim/images/ICRA_Video.png" width="43%" />
  </a>
</p>

Read our paper (https://arxiv.org/pdf/2209.11094.pdf) and cite us.

<a href="https://arxiv.org/pdf/2209.11094.pdf">
  <img src="PRL4AirSim/images/ArxivLogo.png" width="20%" />
</a>
```
@article{saunders2022parallel,
    title={Parallel Reinforcement Learning Simulation for Visual Quadrotor Navigation},
    author={Saunders, Jack and Saeedi, Sajad and Li, Wenbin},
    journal={arXiv preprint arXiv:2209.11094},
    year={2022}
}
```

# What is PRL4AirSim?

PRL4AirSim provides a simulation framework, built on AirSim, which provides efficient parallel training.  We add the following functionality:

1. Batch rendering, to remove multiple game and render thread synchronisation using `simGetBatchImages` to encompas all requests into a single request.
2. Episodic Training, using `resetVehicle` rather than `reset` which causes the entire simulation to reset.
3. Non-interactive simulator environment, removing ego-perspective rendering and agent-agent collision interaction.
4. Ape-X implementation using RPC servers

<img src="PRL4AirSim/images/PRL4AirSimBlockDiagram.png" width="70%" />

## Batch Rendering

Rather than many asynchronous RPC commands, we encompas all requests into a single request `simGetBatchImages`

<img src="PRL4AirSim/images/BatchRenderGraph.png" width="70%" />



# Build PRL4AirSim

## Building AirSim
Build AirSim normally.  **Ensure you clone this repository rather than AirSim, then follow their steps as we have modified the AirLib and AirSim plugins.**
- [Windows Build Link](https://microsoft.github.io/AirSim/build_windows)
- [Linux Build Link](https://microsoft.github.io/AirSim/build_linux)
- [MacOS Build Link](https://microsoft.github.io/AirSim/build_macos)

## Install Python Packages

Find the `requirements.txt` file which includes modules used in our python environment and versions.

The required modules are:
- AirSim
- PyTorch
- Numpy
- OpenCV
- wandb (for logging)
- json
- msgpackrpc (https://github.com/tbelhalfaoui/msgpack-rpc-python/tree/fix-msgpack-dep) see bellow

## Correct msgpackRPC installation

msgpack changed its name from msgpack-python to msgpack, which leads to an outdated version installed.  When running the outdated version, the python instance is used rather than the quicker c++ version.  Hence, we need to correct this using `https://github.com/tbelhalfaoui/msgpack-rpc-python/tree/fix-msgpack-dep`.  If Jupyter package is installed, tornado will also be outdated.  This will lead to the following python package versions:

# AirSim Configuration

One major disavantage of AirSim is the configuration file for the quadrotors within the simulator.  Every quadrotor requires a specific configuration which leads to thousands of repetitive lines within the configuration file located in `/Documents/AirSim/settings.json`.  In the future we hope to automate the `num_drones` setting in our config to initialise the number of quadrotors on the AirSim side.  However, for now we need this setting (`num_drones`) to match the number of vehicles in the `Documents/AirSim/settings.json` file.  Where each quadrotor id and camera id increments:

```json
{
  "SeeDocsAt": "https://github.com/Microsoft/AirSim/blob/master/docs/settings.md",
  "SimMode": "Multirotor",
  "ViewMode": "NoDisplay",
  "SettingsVersion": 1.2,
  "ClockSpeed": 4.0,
  "LocalHostIp": "127.0.0.1",
  "ApiServerPort": 29001,
  "SubWindows": [],
  "Vehicles": {
    "Drone0": {
      "X": 0,
      "Y": 0,
      "Z": 0,
      "VehicleType": "SimpleFlight",
      "AutoCreate": true,
      "DefaultVehicleState": "Armed",
      "Cameras": {
        "depth_cam_0": {
          "CaptureSettings": [
            {
              "FOV_Degrees": 90,
              "ImageType": 1,
              "Width": 32,
              "Height": 32
            }
          ],
          "Pitch": 0.0,
          "Roll": 0.0,
          "Yaw": 0.0,
          "X": 0.0,
          "Y": 0.0,
          "Z": -0.6
        }
      }
    },
    "Drone1": {
      "X": 0,
      "Y": 0,
      "Z": 0,
      "VehicleType": "SimpleFlight",
      "AutoCreate": true,
      "DefaultVehicleState": "Armed",
      "Cameras": {
        "depth_cam_1": {
          "CaptureSettings": [
            {
              "FOV_Degrees": 90,
              "ImageType": 1,
              "Width": 32,
              "Height": 32
            }
          ],
          "Pitch": 0.0,
          "Roll": 0.0,
          "Yaw": 0.0,
          "X": 0.0,
          "Y": 0.0,
          "Z": -0.6
        }
      }
    }
    ...
  }
}
```

Furthermore, we modify the `ApiServerPort` and `LocalHostIp` setting when running the parallel architecture.  We have a set of templates that can be swapped out to test any number of quadrotors up to 60 (although we have personally tested with 200) in `PRL4AirSim/TemplateConfigurationFiles`.

## PRL4AirSim Configuration

We provide a `config.json` file which contains all parameters of the experiment.  

```json
{
  "max_episodes" : 100000,
  "state_space" : [2, 32, 32],
  "action_space" : 2,
  "buffer_Size": 15000,
  "batch_size" : 32,
  "learning_rate" : 0.001,
  "discount_factor" : 0.95,
  "replace_target_count_episode" : 150,
  "num_drones" : 50,
  "from_artifact" : "",
  "projectName" : "MyProject2",
  "envProcesses" : 1,
  "storage_port" : 29000,
  "headless" : false
}
```

We used this framework to test visual navigation, as a result the state space and action space is specific for this task.  Changing this involves modifying `simulator.py`, `DQNTrainer.py`, and `DQNetwork.py`.

# Running PRL4AirSim

A quick way to test our framework is to use the `start.py` python script, which will run a local version of the PRL4AirSim.  However, we recommend to run all subprocesses on networked computers.

There are four components:

| File      | Description |
| ----------- | ----------- |
| `PRL4AirSim/UEBinary/{ProjectName}.sh`      | AirSim Unreal Engine Binary       |
| `PRL4AirSim/PyClient.py`   | PyClient AirSim Client hosting a local DQN network        |
| `PRL4AirSim/Storage.py`   | Replay Buffer       |
| `PRL4AirSim/Trainer.py`   | Global network parameter trainer      |

Alternatively, we recommend running the airsim and local DQN network on multiple compute units as shown in the figure bellow, thus spreading the GPU and CPU load.  This can be done by running the PyClient and UEBinary remotely, ensuring the IP of the buffer is defined on these machines.  Check out `start.py` on how to do this.  

<img src="PRL4AirSim/images/ParallelRLFramework.png" width="70%" />

# PRL4AirSim UnrealEngine and Replay Buffer connection commands

Ape-X enables us to run multiple DQN local instances in parallel (hence parallel RL).  This results in an increased data sample rate from the environmnet.  The bottleneck of AirSim is the high CPU utilisation from the quadrotor dynamics which are calculated every step for each vehicle.  Vectorising the environment can improve memory utilisation 


We have modified the AirLib library to accept new commands that enable vectorised based environment interaction.  Hence, intialise the MultiRotor client normally

**Note: in our code we use client.call(...) without differentiating between the UnrealEngine and Replay buffer.  However, for this documentation simClient means the UnrealEngine client and replayClient is the Replay bufer**

```python
simClient = airsim.MultirotorClient(ue_ip_address : str, ue_port : int)
simClient.confirmConnection()

bufferClient = msgpackrpc.Client(msgpackrpc.Address(buffer_ip_address : str, buffer_port : int))
print(bufferClient.call("confirmConnection"))
```

## PRL4AirSim Agent Simulator Batch commands

```python
simClient.call('simGetBatchImages', requests : [airsim.ImageRequest], vehicle_names : [str])
```

```python
simClient.call_async('moveByVelocityZBatch', vx_vec : [float], vy_vec : [float], z_vec : [float], time : float, driveTrainType : airsim.DrivetrainType, yawMode : airsim.YawMode, vehicle_names : [str])
```

```python
simClient.call('simSetVehiclePoseBatch', poses : [airsim.Pose], vehicle_names : [str])
```

```python
simClient.call_async("resetVehicle", vehicle_name : str, pose : airsim.Pose, orientation : airsim.Quaternionr)
```

## PRL4AirSim Shared Replay Buffer

Similarly to AirSim, we use a RPC server to host the replay buffer.  This buffer is a centralised storage used by the local agent instances to fill experiences to, and an external trainer client which samples experiences.

```python
def pushMemory(self, state, action, next_state, reward, not_done)
```

```python
# For Epislon-Greedy action selection, epsilon can decrement with the size of the experience replay.
def getMemoryPushCounter(self)
```

```python
# There is a delay between when the replay buffer starts and when agents start interacting with the environment.
# We use this to properly define when the experiment starts.
def startSimulation(self)
```

```python
# Used to track episodic training
def finishEpisode(self)
```

```python
# The trainer calls this to sample experiences from the buffer.  We store experiences as a dictionary of numpy arrays 
# which need to be converted to python lists for the RPC server to encode and sent to the trainer client.  See 'Utils.convertStateDicToListDic()'
def sampleFromStorage(self)
```

# Non-interactive Unreal Engine Custom Environments

We provide the environment presented within the paper to allow others to validate our approach.  However, to create a custom environment, we recomend you follow the following steps to prevent agent interaction.

## Remove ego-perspective rendering of other quadrotors

To make the quadrotor invisible in the scene, change the 'Hidden in Scene Capture' to True.  This will make it invisible to other drones but the spectator actor can still see it.  Go to details, then rendering, this will show the setting 'ACtor Hidden In Game'.

<img src="PRL4AirSim/images/NonInteractiveUE/MakeActorHidden.png" width="70%" />

<img src="PRL4AirSim/images/NonInteractiveUE/MakeActorHiddenZoom.png" width="40%" />

## Remove Collision boxes from all agents within the environment

We need to specifically remove agent-agent interaction while also enabling environment interaction.  Hence we need to define all components of the quadrotor blueprint 'BP_FlyingPawn' as 'Pawn' and ignore any overlaps that occour between this group.  To do this, we modify the collision response within the agent blueprint.

There are five components to change within the 'BP_FlyingPawn' blueprint: BodyMesh, Prop3, Prop2, Prop1, Prop0.  For all of these, go to collisions, then change the collision presents to custom.  Thange the Object Type to 'Pawn' and then in 'Object Responses' change the Pawn to Ignore as shown bellow.

<img src="PRL4AirSim/images/NonInteractiveUE/CollisionPresets.png" width="40%" />


Now to remove collisions between 'Pawns', we need to ignore the event 'ActorBeginOverlap' which we can do using a Blueprint Event Graph.  Add the following event graph to 'BP_FlyingPawn'.

<img src="PRL4AirSim/images/NonInteractiveUE/IgnoreCollisionBP.png" width="70%" />


Agents will interact with the environment without interacting with each other.



# Potential Issues and Confusions

1. Correct installation of RPC library.

2. DQNetwork and DQNTrainer used for both the PyClient and Trainer

In the future we will break the functionality of both the DQNetwork and DQNTrainer to make it clearer what functions are used for each. 

3. AirSim and UnrealEngine Coordinate Reference Frame

The reference frame in AirSim is centered around the player start actor.  To spawn agents at certain locations, we capture the position within the unreal engine and then transpose it to the AirSim frame.  Use the function 'convert_pos_UE_to_AS' to convert between UE and AS in Utils.py

```python
def convert_pos_UE_to_AS(origin_UE : np.array, pos_UE : np.array):
```


---



















AirSim is a simulator for drones, cars and more, built on [Unreal Engine](https://www.unrealengine.com/) (we now also have an experimental [Unity](https://unity3d.com/) release). It is open-source, cross platform, and supports software-in-the-loop simulation with popular flight controllers such as PX4 & ArduPilot and hardware-in-loop with PX4 for physically and visually realistic simulations. It is developed as an Unreal plugin that can simply be dropped into any Unreal environment. Similarly, we have an experimental release for a Unity plugin.

Our goal is to develop AirSim as a platform for AI research to experiment with deep learning, computer vision and reinforcement learning algorithms for autonomous vehicles. For this purpose, AirSim also exposes APIs to retrieve data and control vehicles in a platform independent way.

**Check out the quick 1.5 minute demo**

Drones in AirSim

[![AirSim Drone Demo Video](docs/images/demo_video.png)](https://youtu.be/-WfTr1-OBGQ)

Cars in AirSim

[![AirSim Car Demo Video](docs/images/car_demo_video.png)](https://youtu.be/gnz1X3UNM5Y)


## How to Get It

### Windows
[![Build Status](https://github.com/microsoft/AirSim/actions/workflows/test_windows.yml/badge.svg)](https://github.com/microsoft/AirSim/actions/workflows/test_windows.yml)
* [Download binaries](https://github.com/Microsoft/AirSim/releases)
* [Build it](https://microsoft.github.io/AirSim/build_windows)

### Linux
[![Build Status](https://github.com/microsoft/AirSim/actions/workflows/test_ubuntu.yml/badge.svg)](https://github.com/microsoft/AirSim/actions/workflows/test_ubuntu.yml)
* [Download binaries](https://github.com/Microsoft/AirSim/releases)
* [Build it](https://microsoft.github.io/AirSim/build_linux)

### macOS
[![Build Status](https://github.com/microsoft/AirSim/actions/workflows/test_macos.yml/badge.svg)](https://github.com/microsoft/AirSim/actions/workflows/test_macos.yml)
* [Build it](https://microsoft.github.io/AirSim/build_macos)

For more details, see the [use precompiled binaries](docs/use_precompiled.md) document. 

## How to Use It

### Documentation

View our [detailed documentation](https://microsoft.github.io/AirSim/) on all aspects of AirSim.

### Manual drive

If you have remote control (RC) as shown below, you can manually control the drone in the simulator. For cars, you can use arrow keys to drive manually.

[More details](https://microsoft.github.io/AirSim/remote_control)

![record screenshot](docs/images/AirSimDroneManual.gif)

![record screenshot](docs/images/AirSimCarManual.gif)


### Programmatic control

AirSim exposes APIs so you can interact with the vehicle in the simulation programmatically. You can use these APIs to retrieve images, get state, control the vehicle and so on. The APIs are exposed through the RPC, and are accessible via a variety of languages, including C++, Python, C# and Java.

These APIs are also available as part of a separate, independent cross-platform library, so you can deploy them on a companion computer on your vehicle. This way you can write and test your code in the simulator, and later execute it on the real vehicles. Transfer learning and related research is one of our focus areas.

Note that you can use [SimMode setting](https://microsoft.github.io/AirSim/settings#simmode) to specify the default vehicle or the new [ComputerVision mode](https://microsoft.github.io/AirSim/image_apis#computer-vision-mode-1) so you don't get prompted each time you start AirSim.

[More details](https://microsoft.github.io/AirSim/apis)

### Gathering training data

There are two ways you can generate training data from AirSim for deep learning. The easiest way is to simply press the record button in the lower right corner. This will start writing pose and images for each frame. The data logging code is pretty simple and you can modify it to your heart's content.

![record screenshot](docs/images/record_data.png)

A better way to generate training data exactly the way you want is by accessing the APIs. This allows you to be in full control of how, what, where and when you want to log data.

### Computer Vision mode

Yet another way to use AirSim is the so-called "Computer Vision" mode. In this mode, you don't have vehicles or physics. You can use the keyboard to move around the scene, or use APIs to position available cameras in any arbitrary pose, and collect images such as depth, disparity, surface normals or object segmentation.

[More details](https://microsoft.github.io/AirSim/image_apis)

### Weather Effects

Press F10 to see various options available for weather effects. You can also control the weather using [APIs](https://microsoft.github.io/AirSim/apis#weather-apis). Press F1 to see other options available.

![record screenshot](docs/images/weather_menu.png)

## Tutorials

- [Video - Setting up AirSim with Pixhawk Tutorial](https://youtu.be/1oY8Qu5maQQ) by Chris Lovett
- [Video - Using AirSim with Pixhawk Tutorial](https://youtu.be/HNWdYrtw3f0) by Chris Lovett
- [Video - Using off-the-self environments with AirSim](https://www.youtube.com/watch?v=y09VbdQWvQY) by Jim Piavis
- [Webinar - Harnessing high-fidelity simulation for autonomous systems](https://note.microsoft.com/MSR-Webinar-AirSim-Registration-On-Demand.html) by Sai Vemprala
- [Reinforcement Learning with AirSim](https://microsoft.github.io/AirSim/reinforcement_learning) by Ashish Kapoor
- [The Autonomous Driving Cookbook](https://aka.ms/AutonomousDrivingCookbook) by Microsoft Deep Learning and Robotics Garage Chapter
- [Using TensorFlow for simple collision avoidance](https://github.com/simondlevy/AirSimTensorFlow) by Simon Levy and WLU team

## Participate

### Paper

More technical details are available in [AirSim paper (FSR 2017 Conference)](https://arxiv.org/abs/1705.05065). Please cite this as:
```
@inproceedings{airsim2017fsr,
  author = {Shital Shah and Debadeepta Dey and Chris Lovett and Ashish Kapoor},
  title = {AirSim: High-Fidelity Visual and Physical Simulation for Autonomous Vehicles},
  year = {2017},
  booktitle = {Field and Service Robotics},
  eprint = {arXiv:1705.05065},
  url = {https://arxiv.org/abs/1705.05065}
}
```

### Contribute

Please take a look at [open issues](https://github.com/microsoft/airsim/issues) if you are looking for areas to contribute to.

* [More on AirSim design](https://microsoft.github.io/AirSim/design)
* [More on code structure](https://microsoft.github.io/AirSim/code_structure)
* [Contribution Guidelines](CONTRIBUTING.md)

### Who is Using AirSim?

We are maintaining a [list](https://microsoft.github.io/AirSim/who_is_using) of a few projects, people and groups that we are aware of. If you would like to be featured in this list please [make a request here](https://github.com/microsoft/airsim/issues).

## Contact

Join our [GitHub Discussions group](https://github.com/microsoft/AirSim/discussions) to stay up to date or ask any questions.

We also have an AirSim group on [Facebook](https://www.facebook.com/groups/1225832467530667/). 


## What's New

* [Cinematographic Camera](https://github.com/microsoft/AirSim/pull/3949)
* [ROS2 wrapper](https://github.com/microsoft/AirSim/pull/3976)
* [API to list all assets](https://github.com/microsoft/AirSim/pull/3940)
* [movetoGPS API](https://github.com/microsoft/AirSim/pull/3746)
* [Optical flow camera](https://github.com/microsoft/AirSim/pull/3938)
* [simSetKinematics API](https://github.com/microsoft/AirSim/pull/4066)
* [Dynamically set object textures from existing UE material or texture PNG](https://github.com/microsoft/AirSim/pull/3992)
* [Ability to spawn/destroy lights and control light parameters](https://github.com/microsoft/AirSim/pull/3991)
* [Support for multiple drones in Unity](https://github.com/microsoft/AirSim/pull/3128)
* [Control manual camera speed through the keyboard](https://github.com/microsoft/AirSim/pulls?page=6&q=is%3Apr+is%3Aclosed+sort%3Aupdated-desc#:~:text=1-,Control%20manual%20camera%20speed%20through%20the%20keyboard,-%233221%20by%20saihv) 

For complete list of changes, view our [Changelog](docs/CHANGELOG.md)

## FAQ

If you run into problems, check the [FAQ](https://microsoft.github.io/AirSim/faq) and feel free to post issues in the  [AirSim](https://github.com/Microsoft/AirSim/issues) repository.

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.


## License

This project is released under the MIT License. Please review the [License file](LICENSE) for more details.


