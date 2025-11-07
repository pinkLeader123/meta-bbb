# #!/bin/sh
# #-----------------------------------------------------
# # wifi-connect.sh — FINAL PRODUCTION VERSION
# #-----------------------------------------------------
# SSID="Tenda_2085F8"
# PASS=""
# IFACE="wlan0"
# CONF_FILE="/etc/wpa_supplicant.conf"
# LOG_FILE="/var/log/wifi-auto.log"
# LOG_TAG="[wifi-auto]"
# exec >> $LOG_FILE 2>&1
# echo "$(date) $LOG_TAG === BẮT ĐẦU ==="
# # Đảm bảo interface tồn tại
# if ! ip link show $IFACE >/dev/null 2>&1; then
# echo "$LOG_TAG ❌ Không tìm thấy interface $IFACE!"
# exit 1
# fi
# # Kích hoạt Wi-Fi
# ip link set $IFACE up
# sleep 3
# # Cấu hình và chạy wpa_supplicant nếu chưa có
# if ! pgrep -x "wpa_supplicant" >/dev/null; then
# echo "$LOG_TAG 🧩 Khởi động wpa_supplicant..."
# # 1. Tạo nội dung file cấu hình /etc/wpa_supplicant.conf
# echo "$LOG_TAG 📝 Tạo cấu hình wpa_supplicant..."
# # Tiền tố chung cho file cấu hình
# cat > $CONF_FILE << EOF
# ctrl_interface=/var/run/wpa_supplicant
# update_config=1
# EOF
# if [ -z "$PASS" ]; then
# # Trường hợp 1: Mạng KHÔNG MẬT KHẨU (PASS rỗng)
# echo "$LOG_TAG 🔓 Mạng mở được phát hiện (key_mgmt=NONE)."
# # Ghi đè phần cấu hình mạng mở vào file
# cat >> $CONF_FILE << EOF
# network={
#     ssid="$SSID"
#     key_mgmt=NONE
# }
# EOF
# else
# # Trường hợp 2: Mạng CÓ MẬT KHẨU (PASS không rỗng)
# echo "$LOG_TAG 🔒 Mạng có mật khẩu được phát hiện."
# # Dùng wpa_passphrase để hash mật khẩu và thêm vào file
# # Lệnh này tự tạo block 'network={...}' và bao gồm PSK đã hash.
# # wpa_passphrase "$SSID" "$PASS" >> $CONF_FILE
# # Cải tiến: dùng wpa_passphrase, loại bỏ dòng # (comment) và thêm vào file
# wpa_passphrase "$SSID" "$PASS" | grep -v '^\#' >> $CONF_FILE
# fi
# # 2. Khởi động wpa_supplicant với cấu hình đã tạo
# echo "$LOG_TAG 🚀 Chạy tiến trình wpa_supplicant..."
# wpa_supplicant -B -i $IFACE -c $CONF_FILE
# else
# echo "$LOG_TAG ⚙️ wpa_supplicant đã chạy."
# # Nếu wpa_supplicant đã chạy, ta có thể yêu cầu nó đọc lại cấu hình
# wpa_cli -i $IFACE reconfigure >/dev/null 2>&1 || true
# fi
# # Chờ kết nối tới SSID
# for i in $(seq 1 10); do
# if iw $IFACE link | grep -q "Connected"; then
# echo "$LOG_TAG ✅ Đã kết nối tới $SSID (sau $((i*2)) giây)."
# break
# fi
# echo "$LOG_TAG ⏳ Đang chờ kết nối Wi-Fi... ($i/10)"
# sleep 2
# done
# # Lấy IP
# echo "$LOG_TAG 🌐 Yêu cầu IP DHCP..."
# dhcpcd -w $IFACE # <- "-w" block cho đến khi có IP thật sự
# # Kiểm tra IP nhiều lần
# for i in $(seq 1 6); do
# IP_ADDR=$(ip -4 addr show $IFACE | awk '/inet /{print $2}' | cut -d/ -f1)
# if [ -n "$IP_ADDR" ]; then
# echo "$LOG_TAG ✅ Đã có IP: $IP_ADDR"
# if [ -x /usr/bin/ssd1306 ]; then
# echo "$LOG_TAG 🖥️ Hiển thị IP lên OLED..."
# /usr/bin/ssd1306 "$IP_ADDR"
# fi
# if [ -x /usr/bin/genotp ]; then
# echo "$LOG_TAG 🔐 Chạy chương trình genotp..."
# /usr/bin/genotp &
# fi
# if [ -f /usr/bin/senOTP.py ]; then
# echo "$LOG_TAG 🧠 Chạy server senOTP.py..."
# python3 /usr/bin/senOTP.py &
# fi
# echo "$(date) $LOG_TAG === HOÀN TẤT ==="
# exit 0
# fi
# echo "$LOG_TAG ⏳ Đang đợi DHCP (thử $i/6)..."
# sleep 5
# done
# # Nếu không thành công
# echo "$LOG_TAG ❌ Không nhận được IP sau timeout."
# /usr/bin/ssd1306 "0.0.0.0" 2>/dev/null
# echo "$(date) $LOG_TAG === KẾT THÚC (THẤT BẠI) ==="
# exit 1
# #-----------------------------------------------------



#!/bin/sh
#-----------------------------------------------------
# wifi-connect.sh — FINAL PRODUCTION VERSION (với tích hợp OpenSSL)
#-----------------------------------------------------
SSID="Tenda_2085F8"
PASS=""
IFACE="wlan0"
CONF_FILE="/etc/wpa_supplicant.conf"
LOG_FILE="/var/log/wifi-auto.log"
LOG_TAG="[wifi-auto]"
exec >> $LOG_FILE 2>&1
echo "$(date) $LOG_TAG === BẮT ĐẦU ==="
# Đảm bảo interface tồn tại
if ! ip link show $IFACE >/dev/null 2>&1; then
echo "$LOG_TAG ❌ Không tìm thấy interface $IFACE!"
exit 1
fi
# Kích hoạt Wi-Fi
ip link set $IFACE up
sleep 3
# Cấu hình và chạy wpa_supplicant nếu chưa có
if ! pgrep -x "wpa_supplicant" >/dev/null; then
echo "$LOG_TAG 🧩 Khởi động wpa_supplicant..."
# 1. Tạo nội dung file cấu hình /etc/wpa_supplicant.conf
echo "$LOG_TAG 📝 Tạo cấu hình wpa_supplicant..."
# Tiền tố chung cho file cấu hình
cat > $CONF_FILE << EOF
ctrl_interface=/var/run/wpa_supplicant
update_config=1
EOF
if [ -z "$PASS" ]; then
# Trường hợp 1: Mạng KHÔNG MẬT KHẨU (PASS rỗng)
echo "$LOG_TAG 🔓 Mạng mở được phát hiện (key_mgmt=NONE)."
# Ghi đè phần cấu hình mạng mở vào file
cat >> $CONF_FILE << EOF
network={
    ssid="$SSID"
    key_mgmt=NONE
}
EOF
else
# Trường hợp 2: Mạng CÓ MẬT KHẨU (PASS không rỗng)
echo "$LOG_TAG 🔒 Mạng có mật khẩu được phát hiện."
# Dùng wpa_passphrase để hash mật khẩu và thêm vào file
# Lệnh này tự tạo block 'network={...}' và bao gồm PSK đã hash.
# wpa_passphrase "$SSID" "$PASS" >> $CONF_FILE
# Cải tiến: dùng wpa_passphrase, loại bỏ dòng # (comment) và thêm vào file
wpa_passphrase "$SSID" "$PASS" | grep -v '^\#' >> $CONF_FILE
fi
# 2. Khởi động wpa_supplicant với cấu hình đã tạo
echo "$LOG_TAG 🚀 Chạy tiến trình wpa_supplicant..."
wpa_supplicant -B -i $IFACE -c $CONF_FILE
else
echo "$LOG_TAG ⚙️ wpa_supplicant đã chạy."
# Nếu wpa_supplicant đã chạy, ta có thể yêu cầu nó đọc lại cấu hình
wpa_cli -i $IFACE reconfigure >/dev/null 2>&1 || true
fi
# Chờ kết nối tới SSID
for i in $(seq 1 10); do
if iw $IFACE link | grep -q "Connected"; then
echo "$LOG_TAG ✅ Đã kết nối tới $SSID (sau $((i*2)) giây)."
break
fi
echo "$LOG_TAG ⏳ Đang chờ kết nối Wi-Fi... ($i/10)"
sleep 2
done
# Lấy IP
echo "$LOG_TAG 🌐 Yêu cầu IP DHCP..."
dhcpcd -w $IFACE # <- "-w" block cho đến khi có IP thật sự
# Kiểm tra IP nhiều lần
for i in $(seq 1 6); do
IP_ADDR=$(ip -4 addr show $IFACE | awk '/inet /{print $2}' | cut -d/ -f1)
if [ -n "$IP_ADDR" ]; then
echo "$LOG_TAG ✅ Đã có IP: $IP_ADDR"
if [ -x /usr/bin/ssd1306 ]; then
echo "$LOG_TAG 🖥️ Hiển thị IP lên OLED..."
/usr/bin/ssd1306 "$IP_ADDR"
fi
if [ -x /usr/bin/genotp ]; then
echo "$LOG_TAG 🔐 Chạy chương trình genotp..."
/usr/bin/genotp &
python3 /usr/bin/smartfarm &
fi

# TÍCH HỢP: Tạo chứng chỉ SSL tự ký nếu chưa có (trước khi chạy senOTP.py)
CERT_DIR="/usr/bin"
if [ ! -f "$CERT_DIR/server.crt" ]; then
    echo "$LOG_TAG 🔐 Tạo chứng chỉ SSL tự ký cho senOTP.py..."
    cd "$CERT_DIR" || { echo "$LOG_TAG ❌ Không thể cd vào $CERT_DIR!"; exit 1; }
    openssl req -new -x509 -days 365 -nodes -out server.crt -keyout server.key -subj "/CN=datn-bbb-server" || { echo "$LOG_TAG ❌ Lỗi tạo chứng chỉ!"; exit 1; }
    chmod 600 server.key  # Bảo mật private key
    echo "$LOG_TAG ✅ Đã tạo server.crt và server.key trong $CERT_DIR."
else
    echo "$LOG_TAG 📜 Chứng chỉ SSL đã tồn tại, bỏ qua."
fi

if [ -f /usr/bin/senOTP.py ]; then
echo "$LOG_TAG 🧠 Chạy server senOTP.py..."
python3 /usr/bin/senOTP.py &
fi
echo "$(date) $LOG_TAG === HOÀN TẤT ==="
exit 0
fi
echo "$LOG_TAG ⏳ Đang đợi DHCP (thử $i/6)..."
sleep 5
done
# Nếu không thành công
echo "$LOG_TAG ❌ Không nhận được IP sau timeout."
/usr/bin/ssd1306 "0.0.0.0" 2>/dev/null
echo "$(date) $LOG_TAG === KẾT THÚC (THẤT BẠI) ==="
exit 1
#-----------------------------------------------------