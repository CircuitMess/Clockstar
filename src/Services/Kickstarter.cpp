#include "Kickstarter.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

HTTPClient* http;

const char* ca = "-----BEGIN CERTIFICATE-----\n"
				 "MIIMtjCCC56gAwIBAgIMXziw+8a/lfUk3fqlMA0GCSqGSIb3DQEBCwUAMFcxCzAJ\n"
				 "BgNVBAYTAkJFMRkwFwYDVQQKExBHbG9iYWxTaWduIG52LXNhMS0wKwYDVQQDEyRH\n"
				 "bG9iYWxTaWduIENsb3VkU1NMIENBIC0gU0hBMjU2IC0gRzMwHhcNMjAwNjEzMTQz\n"
				 "ODA5WhcNMjEwNDI0MTkyMjM5WjB3MQswCQYDVQQGEwJVUzETMBEGA1UECAwKQ2Fs\n"
				 "aWZvcm5pYTEWMBQGA1UEBwwNU2FuIEZyYW5jaXNjbzEVMBMGA1UECgwMRmFzdGx5\n"
				 "LCBJbmMuMSQwIgYDVQQDDBtmNi5zaGFyZWQuZ2xvYmFsLmZhc3RseS5uZXQwggEi\n"
				 "MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCtg6vXh2tmwTGsbQYnDDTE786l\n"
				 "g22sbW664cZ04/ztlXWNguGiJQZwFmYHt3K5K0ZXgAHBQuY0hWhFJXeUlCkUbpeC\n"
				 "uSN7OYrOk+ThkR40y+QPsRho9zAYaixSBexsyWViPKVHoY+MqqHtTYJOzzunrpcG\n"
				 "VAlk/3oZlIojLyFB7na6IKRa7Gn40B31xFyQUMeJa1EV3ypqQ8TvwBq+FbOoohHJ\n"
				 "Z56cvSgeFJ/m0CaKBiduhEOGGDKifNXzE1ZKVnqoJjBpdQTLIVhgPsSwY1bFrEtB\n"
				 "dqVjv4S3w7GafJqVkBT7uHhjEK632NDhRTwbEh5ALyP0tyw6HhbXjPtLkPR1AgMB\n"
				 "AAGjgglgMIIJXDAOBgNVHQ8BAf8EBAMCBaAwgYoGCCsGAQUFBwEBBH4wfDBCBggr\n"
				 "BgEFBQcwAoY2aHR0cDovL3NlY3VyZS5nbG9iYWxzaWduLmNvbS9jYWNlcnQvY2xv\n"
				 "dWRzc2xzaGEyZzMuY3J0MDYGCCsGAQUFBzABhipodHRwOi8vb2NzcDIuZ2xvYmFs\n"
				 "c2lnbi5jb20vY2xvdWRzc2xzaGEyZzMwVgYDVR0gBE8wTTBBBgkrBgEEAaAyARQw\n"
				 "NDAyBggrBgEFBQcCARYmaHR0cHM6Ly93d3cuZ2xvYmFsc2lnbi5jb20vcmVwb3Np\n"
				 "dG9yeS8wCAYGZ4EMAQICMAkGA1UdEwQCMAAwggbyBgNVHREEggbpMIIG5YIbZjYu\n"
				 "c2hhcmVkLmdsb2JhbC5mYXN0bHkubmV0ggoqLjRzcWkubmV0ghAqLmJvb2tpbmcu\n"
				 "ZXhwZXJ0ghAqLmJyYW5kZm9sZGVyLmlvghAqLmJyaWdodGNvdmUubmV0gg8qLmNo\n"
				 "YXJ0YmVhdC5jb22CCSouZGllei5oboIOKi5lbGhlcmFsZG8uaG6CFiouZmFzdGx5\n"
				 "LnBpY21vbmtleS5jb22CHCouZmFzdGx5LnN0YXRpYy5maXJlYmFzZS5jb22CFyou\n"
				 "ZmFzdGx5LnphZ2ZvcndhcmQuY29tghIqLmZlcmltcG9ydC5jb20uYnKCDiouZmly\n"
				 "ZWJhc2UuY29tghEqLmZsZXhzaG9wcGVyLmNvbYIQKi5mb3Vyc3F1YXJlLmNvbYIR\n"
				 "Ki5nYWR2ZW50dXJlcy5jb22CDCoua2Fub3B5LmNvbYIRKi5raWNrc3RhcnRlci5j\n"
				 "b22CDSoubGFwcmVuc2EuaG6CDSoubWF3ZG9vMy5jb22CDCoubWVyY2RuLm5ldIIU\n"
				 "Ki5tZXJwYXktc2FuZGJveC5jb22CDioubWVycGF5Lm5pbmphghYqLm1peGVydXNl\n"
				 "cmNvbnRlbnQuY29tghIqLm1vbmV5Zm9yd2FyZC5jb22CEyoubXlhcHNhcmF2aWRl\n"
				 "by5jb22CDyoucGljbW9ua2V5LmNvbYIOKi5zZWF0Z2Vlay5jb22CEyouc2tpbXJl\n"
				 "c291cmNlcy5jb22CECouc3BsaXQtc3RhZ2UuaW+CDiouc3FleC1lZGdlLmpwghAq\n"
				 "LnNxdWFyZW9mZnMuY29tghUqLnN0YWdpbmcuZmxleGludC5uZXSCCyoudWplZWIu\n"
				 "Y29tggwqLndlYnRlYi5jb22CDCoud2lraWhvdy5jeoIMKi53aWtpaG93Lml0ggwq\n"
				 "Lndpa2lob3cudm6CCDRzcWkubmV0ghFhY2FkZW15LnNob3Bnby5tZYIbYWRtaW4u\n"
				 "cGVyZm9ybWFuY2VoZWFsdGguY29tgg9icmFuZGZvbGRlci5jb22CDGNjbGVhbmVy\n"
				 "LmNvbYISY2RuLmJyYW5kZm9sZGVyLmlvghVjZG4yLnNoYXJwZXJpbWFnZS5jb22C\n"
				 "FWNkbjMuc2hhcnBlcmltYWdlLmNvbYINY2hhcnRiZWF0LmNvbYINZGViLm9wZXJh\n"
				 "LmNvbYIHZGllei5oboINZHJlYW1ob3N0LmNvbYIRZWRnZS5iaXJjaGJveC5jb22C\n"
				 "F2VkZ2UuZGV2LmJpcmNoYm94LmNvLnVrghVlZGdlLmRldi5iaXJjaGJveC5jb22C\n"
				 "FGVkZ2UuZGV2LmJpcmNoYm94LmVzghtlZGdlLnN0YWdpbmcuYmlyY2hib3guY28u\n"
				 "dWuCGWVkZ2Uuc3RhZ2luZy5iaXJjaGJveC5jb22CGGVkZ2Uuc3RhZ2luZy5iaXJj\n"
				 "aGJveC5lc4IMZWxoZXJhbGRvLmhughNmYXN0LW5jLmFwcGN1ZXMuY29tghRmYXN0\n"
				 "bHkucGljbW9ua2V5LmNvbYIVZmVrcmEuZWZhd2F0ZWVyY29tLmpvghBmZXJpbXBv\n"
				 "cnQuY29tLmJyggxmaXJlYmFzZS5jb22CD2ZsZXhzaG9wcGVyLmNvbYIOZm91cnNx\n"
				 "dWFyZS5jb22CD2dhZHZlbnR1cmVzLmNvbYIRaW1hZ2UuYXBwY3Vlcy5jb22CD2lt\n"
				 "YWdlcy5yLW4taS5qcIIKa2Fub3B5LmNvbYILbGFwcmVuc2EuaG6CGm0ycHJvZC5m\n"
				 "ZXN0b29scHJvZHVjdHMuY29tghJtMnByb2QudG9vbG51dC5jb22CC21hd2RvbzMu\n"
				 "Y29tghRtaXhlcnVzZXJjb250ZW50LmNvbYIUbXkuYmlsbHNwcmluZ2FwcC5jb22C\n"
				 "DXBpY21vbmtleS5jb22CDHNlYXRnZWVrLmNvbYIQc2hhcnBlcmltYWdlLmNvbYIR\n"
				 "c2tpbXJlc291cmNlcy5jb22CDnNwbGl0LXN0YWdlLmlvghBzcG4ub3ptYWxsLmNv\n"
				 "Lmpwgg5zcXVhcmVvZmZzLmNvbYIWc3RhZ2Uuc2hhcnBlcmltYWdlLmNvbYIYc3Rv\n"
				 "cmllcy5icm9zY2hkaXJlY3QuY29tghJzdG9yaWVzLnZlcmludC5jb22CCXVqZWVi\n"
				 "LmNvbYIQd2ViLmNjbGVhbmVyLmNvbYIKd2VidGViLmNvbYIKd2lraWhvdy5jeoIK\n"
				 "d2lraWhvdy5pdIIMd2lraWhvdy50ZWNoggp3aWtpaG93LnZughB3d3cuY2NsZWFu\n"
				 "ZXIuY29tghF3d3cuZHJlYW1ob3N0LmNvbYIQd3d3Lm96bWFsbC5jby5qcIIUd3d3\n"
				 "LnNoYXJwZXJpbWFnZS5jb22CDXd3dy50YXNha2kuZnIwHQYDVR0lBBYwFAYIKwYB\n"
				 "BQUHAwEGCCsGAQUFBwMCMB8GA1UdIwQYMBaAFKkrh+HOJEc7G7/PhTcCVZ0NlFjm\n"
				 "MB0GA1UdDgQWBBR83O9cq2mkks4yPdnKOkkxJhN9gDCCAQMGCisGAQQB1nkCBAIE\n"
				 "gfQEgfEA7wB1AO7Ale6NcmQPkuPDuRvHEqNpagl7S2oaFDjmR7LL7cX5AAABcq4d\n"
				 "3SMAAAQDAEYwRAIga7yJc9Ouig8qh9TVv/Foz6VVawk+ExSSPk+Dk//ODhwCIHdB\n"
				 "hADJ4EqjzfA4vxC15d8Q5UBdadiUvsM2BKU5zEjnAHYA9lyUL9F3MCIUVBgIMJRW\n"
				 "juNNExkzv98MLyALzE7xZOMAAAFyrh3f7wAABAMARzBFAiEA+c8FYLmydOC4AB3S\n"
				 "h0u2xy9BTxg4OYkdeIIVFMRnsqcCIDUBWtLyGcmMdSQKs5pcPOWJ3+4UVxRBsVDH\n"
				 "SWDjoH5MMA0GCSqGSIb3DQEBCwUAA4IBAQA7AnIasGDZZ+MTVXRZQuqAFrG0BT4y\n"
				 "ZCGB8IzOPisD4Fpqfm5+nr/5zV2BzIk8qAWArSaXC7sw1nJUhOVpJBF7r3qfKohX\n"
				 "MgXeH/yQDV1EqylfiiusUcp1zj4GpFMD86yeF111+OKuAJ+schKTG8c17f4jaD6P\n"
				 "0MtAmLWqlcCUx8rJgZokLUtiQgmh88XJ8QCo3mW3REcMboi0XiEcbRCyZIcZKlHe\n"
				 "94x/AvakbOvV6YxRisGaPt2gpEc8vHONpqxA5ryMYZPP5+r8srRNzlg/bP6aUsKh\n"
				 "dlPhcL46vvyBWAVRfP/ID/T+T9jDf/+ogG7S+5/GqjEg3+6g17OFnqRz\n"
				 "-----END CERTIFICATE-----\n";

Kickstarter::Kickstarter(){
	WiFi.begin("CircuitMess", "MAKERphone!");
	while(WiFi.status() != WL_CONNECTED);

	http = new HTTPClient();
	http->addHeader("accept-encoding", "identity");
	http->setReuse(true);
	http->begin("https://www.kickstarter.com/projects/albertgajsak/circuitmess-stem-box/stats.json?v=1", ca);
}

void Kickstarter::update(){
	int code = http->GET();
	Serial.printf("Kickstarter updated, code %d\n", code);
	http->writeToStream(&Serial);

	if(code < 0) return;

	DynamicJsonDocument doc(1000);
	deserializeJson(doc, http->getStream());

	Serial.printf("\nSize: %ld\n", doc.size());

	data.moneys = doc["project"]["pledged"].as<double>();
	data.backers = doc["project"]["backers_count"].as<uint>();
	data.comments = doc["project"]["comments_count"].as<uint>();
}

const CampaignData& Kickstarter::getData() const{
	return data;
}
