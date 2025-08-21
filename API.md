# AI Speaker API 文档

## 接口概述

本文档描述了AI Speaker项目的所有API接口，用于实现音箱控制、音乐播放等功能。

## 数据格式

所有接口均使用JSON格式进行数据传输，编码方式为UTF-8。

## API接口详细说明

### 1. 音箱上报数据
- **命令**: `info`
- **请求格式**:
```json
{
    "cmd": "info",
    "cur_music": "1.mp3",
    "volume": 90,
    "mode": "circle",
    "deviceid": "0001"
}
```
- **参数说明**:
  - `cmd`: 固定值 "info"
  - `cur_music`: 当前播放的音乐文件名
  - `volume`: 音量大小(0-100)
  - `mode`: 播放模式，如 "circle"（循环播放）
  - `deviceid`: 设备ID

### 2. APP上报数据
- **命令**: `app_info`
- **请求格式**:
```json
{
    "cmd": "app_info",
    "appid": "1001",
    "deviceid": "0001"
}
```
- **参数说明**:
  - `cmd`: 固定值 "app_info"
  - `appid`: 应用ID，用于标识不同用户
  - `deviceid`: 设备ID，用于与设备配对

### 3. 请求音乐数据
#### 请求
- **命令**: `get_music_list`
- **请求格式**:
```json
{
    "cmd": "get_music_list",
    "singer": "random"
}
```
- **参数说明**:
  - `cmd`: 固定值 "get_music_list"
  - `singer`: 歌手名称，"random" 表示随机

#### 响应
- **响应格式**:
```json
{
    "cmd": "reply_music",
    "music": ["random/1.mp3", "random/2.mp3", "random/3.mp3", "random/4.mp3", "random/5.mp3"]
}
```

### 4. 上传音乐数据
- **命令**: `upload_music`
- **请求格式**:
```json
{
    "cmd": "upload_music",
    "music": ["1.mp3", "2.mp3", "3.mp3", "4.mp3", "5.mp3"]
}
```
- **说明**: 用于更新APP的音乐列表，通过服务器转发

### 5. 开始播放

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_start"
}
```

#### 音箱响应流程
1. 音箱在线时：
   - 转发数据 `{"cmd":"app_start"}`
   
2. 音箱离线时：
   - 返回服务器，服务器再转发给APP: 
   ```json
   {
       "cmd": "app_start_reply",
       "result": "offline"
   }
   ```

#### 播放结果响应
- **成功响应**:
```json
{
    "cmd": "app_start_reply",
    "result": "success"
}
```
- **失败响应**:
```json
{
    "cmd": "app_start_reply",
    "result": "failure"
}
```

### 6. 结束播放

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_stop"
}
```

#### 音箱响应流程
1. 音箱在线时：
   - 转发数据 `{"cmd":"app_stop"}`
   
2. 音箱离线时：
   - 返回服务器，服务器再转发给APP: 
   ```json
   {
       "cmd": "app_stop_reply",
       "result": "offline"
   }
   ```

#### 停止播放结果响应
- **成功响应**:
```json
{
    "cmd": "app_stop_reply",
    "result": "success"
}
```

### 7. 暂停播放

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_suspend"
}
```

#### 音箱响应流程
1. 音箱在线时：
   - 转发数据 `{"cmd":"app_suspend"}`
   
2. 音箱离线时：
   - 返回服务器，服务器再转发给APP: 
   ```json
   {
       "cmd": "app_suspend_reply",
       "result": "offline"
   }
   ```

#### 暂停播放结果响应
- **成功响应**:
```json
{
    "cmd": "app_suspend_reply",
    "result": "success"
}
```

### 8. 继续播放

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_continue"
}
```

#### 音箱响应流程
1. 音箱在线时：
   - 转发数据 `{"cmd":"app_continue"}`
   
2. 音箱离线时：
   - 返回服务器，服务器再转发给APP: 
   ```json
   {
       "cmd": "app_continue_reply",
       "result": "offline"
   }
   ```

#### 继续播放结果响应
- **成功响应**:
```json
{
    "cmd": "app_continue_reply",
    "result": "success"
}
```

### 9. 上一首

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_prior"
}
```

#### 音箱响应流程
1. 音箱在线时：
   - 转发数据 `{"cmd":"app_prior"}`
   
2. 音箱离线时：
   - 返回服务器，服务器再转发给APP: 
   ```json
   {
       "cmd": "app_prior_reply",
       "result": "offline"
   }
   ```

#### 切换上一首结果响应
- **成功响应**:
```json
{
    "cmd": "app_prior_reply",
    "result": "success",
    "music": "1.mp3"
}
```

### 10. 下一首

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_next"
}
```

#### 音箱响应流程
1. 音箱在线时：
   - 转发数据 `{"cmd":"app_next"}`
   
2. 音箱离线时：
   - 返回服务器，服务器再转发给APP: 
   ```json
   {
       "cmd": "app_next_reply",
       "result": "offline"
   }
   ```

#### 切换下一首结果响应
- **成功响应**:
```json
{
    "cmd": "app_next_reply",
    "result": "success",
    "music": "1.mp3"
}
```

### 11. 增加音量

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_voice_up"
}
```

#### 音箱响应流程
1. 音箱在线时：
   - 转发数据 `{"cmd":"app_voice_up"}`
   
2. 音箱离线时：
   - 返回服务器，服务器再转发给APP: 
   ```json
   {
       "cmd": "app_voice_up_reply",
       "result": "offline"
   }
   ```

#### 增加音量结果响应
- **成功响应**:
```json
{
    "cmd": "app_voice_up_reply",
    "result": "success",
    "voice": 80
}
```

### 12. 减小音量

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_voice_down"
}
```

#### 音箱响应流程
1. 音箱在线时：
   - 转发数据 `{"cmd":"app_voice_down"}`
   
2. 音箱离线时：
   - 返回服务器，服务器再转发给APP: 
   ```json
   {
       "cmd": "app_voice_down_reply",
       "result": "offline"
   }
   ```

#### 减小音量结果响应
- **成功响应**:
```json
{
    "cmd": "app_voice_down_reply",
    "result": "success",
    "voice": 80
}
```

### 13. 单曲循环

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_circle"
}
```

#### 音箱响应流程
1. 音箱在线时：
   - 转发数据 `{"cmd":"app_circle"}`
   
2. 音箱离线时：
   - 返回服务器，服务器再转发给APP: 
   ```json
   {
       "cmd": "app_circle",
       "result": "offline"
   }
   ```

#### 设置单曲循环结果响应
- **成功响应**:
```json
{
    "cmd": "app_circle",
    "result": "success"
}
```

### 14. 顺序播放

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_sequence"
}
```

#### 音箱响应流程
1. 音箱在线时：
   - 转发数据 `{"cmd":"app_sequence"}`
   
2. 音箱离线时：
   - 返回服务器，服务器再转发给APP: 
   ```json
   {
       "cmd": "app_sequence",
       "result": "offline"
   }
   ```

#### 设置顺序播放结果响应
- **成功响应**:
```json
{
    "cmd": "app_sequence",
    "result": "success"
}
```

## 状态码说明

| 状态 | 说明 |
|------|------|
| success | 操作成功 |
| failure | 操作失败 |
| offline | 设备离线 |

## 注意事项

1. 所有命令都必须包含 `cmd` 字段
2. 音量范围为0-100
3. 文件名支持的格式为mp3
4. 设备ID格式为4位数字字符串
5. 在发送新命令前，请确保设备在线状态
6. 暂停播放后可以使用继续播放功能恢复
7. 切换上一首/下一首时会返回新的音乐文件名
8. 音量调节操作会返回调整后的音量值
9. 播放模式包括单曲循环和顺序播放两种方式

### 15. APP注册

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_register",
    "appid": "1001",
    "password": "11111"
}
```
- **参数说明**:
  - `cmd`: 固定值 "app_register"
  - `appid`: 应用ID，用于唯一标识APP
  - `password`: APP注册密码

#### 注册结果响应
- **注册成功响应**:
```json
{
    "cmd": "app_register_reply",
    "result": "success"
}
```
- **注册失败响应**:
```json
{
    "cmd": "app_register_reply",
    "result": "failure"
}
```

### 16. APP登录

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_login",
    "appid": "1001",
    "password": "11111"
}
```
- **参数说明**:
  - `cmd`: 固定值 "app_login"
  - `appid`: 应用ID
  - `password`: APP登录密码

#### 登录结果响应
- **登录成功未绑定设备**:
```json
{
    "cmd": "app_login_reply",
    "result": "not_bind"
}
```
- **登录成功且已绑定设备**:
```json
{
    "cmd": "app_login_reply",
    "result": "bind",
    "deviceid": "0001"
}
```
- **用户不存在**:
```json
{
    "cmd": "app_login_reply",
    "result": "not_exist"
}
```
- **密码错误**:
```json
{
    "cmd": "app_login_reply",
    "result": "password_error"
}
```

### 17. APP绑定

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_bind",
    "appid": "1001",
    "deviceid": "0001"
}
```
- **参数说明**:
  - `cmd`: 固定值 "app_bind"
  - `appid`: 应用ID，用于唯一标识APP
  - `deviceid`: 设备ID，用于绑定指定设备

#### 绑定结果响应
- **绑定成功响应**:
```json
{
    "cmd": "app_bind_reply",
    "result": "success"
}
```

### 18. APP下线

#### APP发起请求
- **请求格式**:
```json
{
    "cmd": "app_offline",
}
```
- **参数说明**:
  - `cmd`: 固定值 "app_offline"

## 更新记录

- 2025-08-20: 
  - 更新详细接口信息，添加具体请求和响应格式
  - 增加结束播放接口说明
  - 增加暂停播放和继续播放接口说明
  - 增加上一首和下一首切换接口说明
  - 增加音量调节接口说明
  - 增加播放模式设置接口说明
- 2025-08-21:
  - 添加APP注册接口说明
  - 添加APP登录接口说明
  - 添加APP绑定接口说明
