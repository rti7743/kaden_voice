iremocon_host = getconfig("setting__aircon_host","192.168.10.2");
iremocon_port = getconfig("setting__aircon_port","51013");
iremocon_second_key_wait_msleep = getconfig("setting___iremocon_second_key_wait_msleep","500");

function call(a1,a2,a3)
	telnet(iremocon_host,iremocon_port,"","*is;"..a1.."\r\n","");
	if a2 then
		msleep(iremocon_second_key_wait_msleep)
		telnet(iremocon_host,iremocon_port,"","*is;"..a2.."\r\n","");
		if a3 then
			msleep(iremocon_second_key_wait_msleep)
			telnet(iremocon_host,iremocon_port,"","*is;"..a3.."\r\n","");
		end
	end
end
