# Deceit-AntiCheat
This is simple Anti-Cheat for game Deceit.
# How to sign your driver:
[Download Windows SDK](https://developer.microsoft.com/ru-ru/windows/downloads/windows-10-sdk/)<br/>
[Cross-Certificates can be find here](https://docs.microsoft.com/en-us/windows-hardware/drivers/install/cross-certificates-for-kernel-mode-code-signing?ranMID=24542&ranEAID=je6NUbpObpQ&ranSiteID=je6NUbpObpQ-effNB8Z64HmvyH.b9JXWFg&epi=je6NUbpObpQ-effNB8Z64HmvyH.b9JXWFg&irgwc=1&OCID=AID2000142_aff_7593_1243925&tduid=%28ir__a2tyedfzegkftxebkk0sohz3zv2xiwzxc0vtlumb00%29%287593%29%281243925%29%28je6NUbpObpQ-effNB8Z64HmvyH.b9JXWFg%29%28%29&irclickid=_a2tyedfzegkftxebkk0sohz3zv2xiwzxc0vtlumb00)
# Signing process:
 * signtool.exe sign /a /ac "any Cross-Certificates.cer" /f "your amazing certificate which you bought.pfx" /p "password from your certificate" yourdrivername.sys
# Test Driver:
[Download](https://github.com/Qurcx01/Deceit-AntiCheat/blob/master/DriverTest/DeceitAntiCheat.sys)
