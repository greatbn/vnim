# viEngine

Khi bắt đầu mình viết engine này với mục đích cho vui với ý tưởng là bỏ dấu được tự do. 
### 1. Lý thuyết:
Để được như vậy, mình có một số giả thiết như sau:
- Một chữ cái tiếng Việt sẽ bắt đầu từ 1 từ chữ cái gốc + 1 phép bỏ dấu
- Một từ tiếng Việt sẽ tập hợp các cữ cái tiếng Việt + 1 phép bỏ dấu
- Mình xử lý tiếng Việt trong phạm ly từ

### 2. Code:

```C
typedef struct {
    UChar origin;
    UChar transform;
    UChar isUpper;
} VNChar;

typedef struct {
    VNChar chars[WORD_MAX_LENGTH + 1];
    UChar transform;
    int length;
} VNWord;
```

### 3. Kết quả:

Dấu của từ sẽ được tự do hơn, không bắt buộc phải đứng với ký tự nào. Khi đó, ta có:
- hojacw = hoặc
- gifow = giờ
- huyefen = huyền

### 4. Nhược điểm:

Gặp vấn đề khi gõ những từ dài không phải là từ tiếng Việt, có nhiều dấu của từ. kiểu như:  "HàHà". 
Tuy nhiên những trường hợp này ít xảy ra khi gõ tiếng Việt. 
Mình hoàn toàn có thể ấn 1 phím đặc biệt (Ctrl)   để báo kiết thúc từ rồi gõ tiếp.

_Hiện tại dự án vẫn đang được phát triển để hoàn thiện._
