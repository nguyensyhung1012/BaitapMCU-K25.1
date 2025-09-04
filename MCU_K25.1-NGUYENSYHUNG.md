**1.** **Giới thiệu khái niệm cơ bản về CISC (Complex Instruction Set Computer) và RISC (Reduced Instruction Set Computer).**

**1.1 . CISC:** (Complex Instruction Set Computer) (Máy tính có tập lệnh phức tạp), là một tập lệnh được sử dụng trong thiết kế kiến ​​trúc máy tính, cung cấp khả năng giao tiếp động hơn. CISC có khả năng tiếp nhận các tập lệnh lớn và đa dạng, mỗi tập lệnh có thể thực hiện nhiều phép toán cấp thấp. Ví dụ về các phép toán này bao gồm truy cập bộ nhớ, số học và logic, tất cả đều có thể được thực hiện trong một lệnh duy nhất. Nhờ luồng và thiết kế này, CISC có thể thực hiện các tác vụ phức tạp hơn với ít lệnh hơn, dẫn đến thiết kế lập trình đơn giản hơn nhiều. CISC thường được tìm thấy trong kiến ​​trúc x86, thường được sử dụng trong máy tính cá nhân.

**1.2. RISC:** (Reduced Instruction Set Computer) (Máy tính với tập lệnh rút gọn), được định nghĩa rõ nhất là một bộ xử lý máy tính hoạt động để đơn giản hóa và thực thi các lệnh, sử dụng các tập lệnh nhỏ, được tối ưu hóa cao để thực thi lệnh nhanh chóng. RISC được sử dụng trong thiết kế kiến ​​trúc máy tính và biến những ý tưởng phức tạp thành thông tin đơn giản hơn để CPU xử lý. Nhờ tính đơn giản của RISC, các tập lệnh được xử lý nhanh hơn nhiều và thường mang lại hiệu suất và hiệu suất cao hơn cho thiết bị sử dụng thiết kế này. Trên thực tế, RISC thường được tìm thấy trong nhiều thiết kế khác nhau cho thiết bị di động, thiết bị chạy bằng pin và hệ thống nhúng .

**2. Trình bày ưu điểm và nhược điểm của từng loại kiến trúc.**

**2.1. CISC**

**Ưu điểm:**

- Chương trình thường ngắn gọn hơn , sử dụng bộ nhớ hiệu quả hơn vì một lệnh có thể thực hiện nhiều công việc. giúp lập trình viên dễ viết mã máy hơn.
- Phù hợp với các ứng dụng truyền thống, hệ điều hành, máy tính để bàn.

**Nhược điểm:**

- Do lệnh phức tạp nên thời gian giải mã dài hơn.
- Tiêu thụ điện năng nhiều hơn, thời gian thực thi chậm hơn.
- Phần cứng phức tạp, tốn nhiều transistor hơn cho mạch điều khiển.

**2.2. RISC**

Ưu điểm:

- Tập lệnh đơn giản , tốc độ thực thi nhanh hơn.
- Phần cứng đơn giản, tiết kiệm năng lượng, thích hợp cho các thiết bị di động, nhúng.

Nhược điểm:

- Do mỗi lệnh đơn giản nên chương trình cần nhiều lệnh hơn, dẫn đến dung lượng lưu trữ lớn.
- Sử dụng các tập lệnh đơn giản hóa cũng làm giảm tính linh hoạt của nó đối với các tác vụ chuyên biệt đòi hỏi các lệnh phức tạp hơn.

**3. So sánh CISC và RISC theo các tiêu chí**

**3.1. Cấu trúc tập lệnh**

- **CISC:** Hàng trăm lệnh, nhiều chế độ địa chỉ phức tạp.
- **RISC:** Ít lệnh (khoảng vài chục), đơn giản, số chế độ địa chỉ hạn chế.

**3.2. Tốc độ xử lý**

- **CISC:** Lệnh phức tạp, mất nhiều thời gian xử lý hơn
- **RISC:** Lệnh đơn giản, tốn ít thời gian xử lý hơn. 

**3.3. Kích thước chương trình**

- **CISC:** Chương trình ngắn gọn hơn do mỗi lệnh “làm được nhiều việc”.
- **RISC:** Chương trình dài hơn vì cần nhiều lệnh hơn để thực hiện cùng tác vụ.

**3.4. Độ phức tạp phần cứng**

- **CISC:** Mạch giải mã phức tạp, cần nhiều transistor, tiêu thụ năng lượng nhiều.
- **RISC:** Thiết kế đơn giản, tiêu thụ năng lượng ít hơn.

**3.5. Ứng dụng thực tế**

- **CISC:** Intel x86, AMD (máy tính để bàn, máy chủ).
- **RISC:** ARM (smartphone, tablet, hệ thống nhúng), MIPS, RISC-V (IoT, nghiên cứu, thiết bị nhúng tiết kiệm năng lượng).

**4. Quan điểm cá nhân** 

Trong bối cảnh hiện nay, các hệ thống nhúng yêu cầu hiệu suất cao đi kèm với mức tiêu thụ năng lượng thấp. Vì vậy, kiến trúc RISC đang chiếm ưu thế.

Lý do:

RISC có phần cứng đơn giản, tiết kiệm năng lượng, rất phù hợp cho thiết bị IoT, vi điều khiển, điện thoại di động.

Nhờ thiết kế tập lệnh đơn giản, CPU dễ mở rộng và tích hợp nhiều lõi hơn , đáp ứng xu hướng đa nhân và điện toán tiết kiệm năng lượng.

Trong khi đó, CISC tiêu tốn lượng lớn năng lượng và chi phí sản xuất phần cứng cao nên không phù hợp bằng trong lĩnh vực nhúng.

