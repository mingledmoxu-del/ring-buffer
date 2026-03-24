# 环形缓冲区从零构建挑战：高性能异步日志器

## 1. 任务背景
在嵌入式开发中，日志打印往往比逻辑运行慢得多。为了不让打印操作阻塞主程序的运行，我们需要实现一个**异步日志缓冲区**。

这个缓冲区的核心是一个**环形缓冲（Ring Buffer）**：
- 当缓冲区有空间时，新日志直接存入。
- **核心挑战**：当缓冲区满时，新日志必须**覆盖掉最旧的一条日志**，并确保旧日志占用的动态内存被正确释放，避免内存泄漏。

---

## 2. 数据结构定义
请在你的代码中使用以下结构定义：

```c
typedef struct {
    uint64_t seq_no;   // 全局递增的唯一序号
    char* content;     // 动态分配的日志内容字符串
} log_item_t;

typedef struct {
    log_item_t* slots; // 存储 log_item_t 的数组
    uint32_t capacity; // 缓冲区总容量
    uint32_t head;     // 指向最旧数据的下标
    uint32_t count;    // 当前缓冲区中的有效条目数
    uint64_t next_seq; // 下一个可用的全局序号
} log_ring_t;
```

---

## 3. 接口规范 (需实现)

### 1) 初始化
`log_ring_t* log_ring_init(uint32_t capacity);`
- 分配 `log_ring_t` 及内部 `slots` 数组内存。
- 初始化所有计数器为 0。

### 2) 压入日志 (核心逻辑)
`void log_ring_push(log_ring_t* ring, const char* text);`
- **逻辑流**：
    1. 使用 `strdup` 或 `malloc+strcpy` 为 `text` 分配新内存。
    2. 如果 `count == capacity` (已满)：
       - 释放 `head` 所指位置的 `content`。
       - 将 `head` 向后移动一位（注意环形回绕）。
       - 减少 `count`（或者覆盖逻辑中直接复用位置）。
    3. 计算插入位置：`index = (head + count) % capacity`。
    4. 存入新产生的 `log_item_t`（包含当前 `next_seq`）。
    5. `next_seq` 递增，`count` 递增（如果不超过容量）。

### 3) 弹出日志
`log_item_t log_ring_pop(log_ring_t* ring);`
- 从 `head` 位置取出一帧。
- `head` 后移，`count` 减一。
- 如果队列为空，返回一个 `content == NULL` 的空对象。

### 4) 销毁
`void log_ring_deinit(log_ring_t* ring);`
- 遍历所有有效条目并 `free(content)`。
- 释放 `slots` 和 [ring](file:///d:/cay/dma_buf/my_dma_buf/dma-buf/dma_buf_stream_ring_demo.c#88-126) 结构。

---

## 4. 验证场景示例
编写一个 [main](file:///d:/cay/dma_buf/my_dma_buf/dma-buf/dma_buf_stream_ring_demo.c#127-245) 函数，执行以下步骤：
1. 初始化容量为 **3** 的缓冲区。
2. 依次压入：`"Alpha"`, `"Beta"`, `"Gamma"`, `"Delta"`。
3. **预期**：压入 `"Delta"` 时，`"Alpha"` 应该被自动释放并覆盖。
4. 连续调用 3 次 `pop`，观察输出的序号和内容。
5. **预期输出**：
   - `[2] Beta`
   - `[3] Gamma`
   - `[4] Delta`
6. 最后销毁缓冲区，检查是否有内存泄漏（建议使用 `valgrind` 或简单的计数观察）。

---

## 5. 进阶挑战 (选做)
- **多线程安全**：引入 `pthread_mutex_t` 确保 `push` 和 `pop` 是线程安全的。
- **引用计数**：模仿 `dma_buf_frame_t` 的逻辑，给 `log_item_t` 增加引用计数，只有当计数归零时才真正释放内容。
