MAKESWF = makeswf -s 600x400 -r 30 -c 0
AS3COMP = as3compile -X 600 -Y 400 -r 30
EDITSWFV = python $(CURDIR)/editswfv.py
RMF = rm -f


all: universal-as2 \
	prefixed-as2 prefixed-as3 \
	multipleargs-as2 multipleargs-as3 \
	selfsending-as2 selfsending-as3 \
	strnullbyte-as2 strnullbyte-as3 \
	messenger-as2 messenger-as3 \
	restrict-as2 restrict-as3 \
	allowdomain-as3 \
	types-as3 \
	allversions-as2 allversions-as3


clean: universal-as2-clean \
	prefixed-as2-clean prefixed-as3-clean \
	multipleargs-as2-clean multipleargs-as3-clean \
	selfsending-as2-clean selfsending-as3-clean \
	strnullbyte-as2-clean strnullbyte-as3-clean \
	messenger-as2-clean messenger-as3-clean \
	restrict-as2-clean restrict-as3-clean \
	allowdomain-as3-clean \
	types-as3-clean \
	allversions-as2-clean allversions-as3-clean


universal-as2:
	$(MAKESWF) -v 8 \
		-o $(CURDIR)/universal/as2/universal-as2.swf \
		$(CURDIR)/universal/as2/universal-as2.as
	$(MAKESWF) -v 8 \
		-o $(CURDIR)/universal/as2/universal-prefixed-as2.swf \
		$(CURDIR)/universal/as2/universal-prefixed-as2.as
	$(MAKESWF) -v 8 \
		-n network \
		-o $(CURDIR)/universal/as2/universal-localwithnetwork-as2.swf \
		$(CURDIR)/universal/as2/universal-as2.as

universal-as2-clean:
	$(RMF) $(CURDIR)/universal/as2/*.pp
	$(RMF) $(CURDIR)/universal/as2/*.swf


prefixed-as2:
	$(MAKESWF) -v 8 \
		-o $(CURDIR)/prefixed/as2/sender-prefixed-as2.swf \
		$(CURDIR)/prefixed/as2/sender-prefixed-as2.as

prefixed-as2-clean:
	$(RMF) $(CURDIR)/prefixed/as2/*.pp
	$(RMF) $(CURDIR)/prefixed/as2/*.swf

prefixed-as3:
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/prefixed/as3/sender-prefixed-as3.swf \
		$(CURDIR)/prefixed/as3/sender-prefixed-as3.as

prefixed-as3-clean:
	$(RMF) $(CURDIR)/prefixed/as3/*.swf


multipleargs-as2:
	$(MAKESWF) -v 8 \
		-o $(CURDIR)/multipleargs/as2/multipleargs-receiver-as2.swf \
		$(CURDIR)/multipleargs/as2/multipleargs-receiver-as2.as
	$(MAKESWF) -v 8 \
		-o $(CURDIR)/multipleargs/as2/multipleargs-sender-as2.swf \
		$(CURDIR)/multipleargs/as2/multipleargs-sender-as2.as

multipleargs-as2-clean:
	$(RMF) $(CURDIR)/multipleargs/as2/*.pp
	$(RMF) $(CURDIR)/multipleargs/as2/*.swf

multipleargs-as3:
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/multipleargs/as3/multipleargs-receiver-as3.swf \
		$(CURDIR)/multipleargs/as3/multipleargs-receiver-as3.as
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/multipleargs/as3/multipleargs-sender-as3.swf \
		$(CURDIR)/multipleargs/as3/multipleargs-sender-as3.as

multipleargs-as3-clean:
	$(RMF) $(CURDIR)/multipleargs/as3/*.swf


selfsending-as2:
	$(MAKESWF) -v 8 \
		-o $(CURDIR)/selfsending/as2/selfsending-as2.swf \
		$(CURDIR)/selfsending/as2/selfsending-as2.as

selfsending-as2-clean:
	$(RMF) $(CURDIR)/selfsending/as2/*.pp
	$(RMF) $(CURDIR)/selfsending/as2/*.swf

selfsending-as3:
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/selfsending/as3/selfsending-as3.swf \
		$(CURDIR)/selfsending/as3/selfsending-as3.as

selfsending-as3-clean:
	$(RMF) $(CURDIR)/selfsending/as3/*.swf


strnullbyte-as2:
	$(MAKESWF) -v 8 \
		-o $(CURDIR)/strnullbyte/as2/strnullbyte-as2.swf \
		$(CURDIR)/strnullbyte/as2/strnullbyte-as2.as

strnullbyte-as2-clean:
	$(RMF) $(CURDIR)/strnullbyte/as2/*.pp
	$(RMF) $(CURDIR)/strnullbyte/as2/*.swf

strnullbyte-as3:
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/strnullbyte/as3/strnullbyte-as3.swf \
		$(CURDIR)/strnullbyte/as3/strnullbyte-as3.as

strnullbyte-as3-clean:
	$(RMF) $(CURDIR)/strnullbyte/as3/*.swf


messenger-as2:
	$(MAKESWF) -v 8 \
		-o $(CURDIR)/messenger/as2/messenger1-as2.swf \
		$(CURDIR)/messenger/as2/messenger1-as2.as
	$(MAKESWF) -v 8 \
		-o $(CURDIR)/messenger/as2/messenger2-as2.swf \
		$(CURDIR)/messenger/as2/messenger2-as2.as

messenger-as2-clean:
	$(RMF) $(CURDIR)/messenger/as2/*.pp
	$(RMF) $(CURDIR)/messenger/as2/*.swf

messenger-as3:
	$(AS3COMP) -T 9 \
		-I $(CURDIR)/messenger/as3 \
		-o $(CURDIR)/messenger/as3/messenger1-as3.swf \
		$(CURDIR)/messenger/as3/messenger1-as3.as
	$(AS3COMP) -T 9 \
		-I $(CURDIR)/messenger/as3 \
		-o $(CURDIR)/messenger/as3/messenger2-as3.swf \
		$(CURDIR)/messenger/as3/messenger2-as3.as
	$(AS3COMP) -T 9 \
		-I $(CURDIR)/messenger/as3 \
		-o $(CURDIR)/messenger/as3/messenger1-peruser-as3.swf \
		$(CURDIR)/messenger/as3/messenger1-peruser-as3.as
	$(AS3COMP) -T 9 \
		-I $(CURDIR)/messenger/as3 \
		-o $(CURDIR)/messenger/as3/messenger2-peruser-as3.swf \
		$(CURDIR)/messenger/as3/messenger2-peruser-as3.as

messenger-as3-clean:
	$(RMF) $(CURDIR)/messenger/as3/*.swf


restrict-as2:
	$(MAKESWF) -v 8 \
		-n file \
		-o $(CURDIR)/restrictfilenetwork/as2/restrict-file-as2.swf \
		$(CURDIR)/restrictfilenetwork/as2/restrict-file-as2.as
	$(MAKESWF) -v 8 \
		-n network \
		-o $(CURDIR)/restrictfilenetwork/as2/restrict-network-as2.swf \
		$(CURDIR)/restrictfilenetwork/as2/restrict-network-as2.as

restrict-as2-clean:
	$(RMF) $(CURDIR)/restrictfilenetwork/as2/*.pp
	$(RMF) $(CURDIR)/restrictfilenetwork/as2/*.swf

restrict-as3:
	$(AS3COMP) -T 9 \
		-L \
		-I $(CURDIR)/restrictfilenetwork/as3 \
		-o $(CURDIR)/restrictfilenetwork/as3/restrict-file-as3.swf \
		$(CURDIR)/restrictfilenetwork/as3/restrict-file-as3.as
	$(AS3COMP) -T 9 \
		-N \
		-I $(CURDIR)/restrictfilenetwork/as3 \
		-o $(CURDIR)/restrictfilenetwork/as3/restrict-network-as3.swf \
		$(CURDIR)/restrictfilenetwork/as3/restrict-network-as3.as

restrict-as3-clean:
	$(RMF) $(CURDIR)/restrictfilenetwork/as3/*.swf


allowdomain-as3:
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/allowdomain/as3/allowdomain-asterisk-as3.swf \
		$(CURDIR)/allowdomain/as3/allowdomain-asterisk-as3.as
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/allowdomain/as3/allowinsecuredomain-asterisk-as3.swf \
		$(CURDIR)/allowdomain/as3/allowinsecuredomain-asterisk-as3.as
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/allowdomain/as3/allowdomain-localhost-as3.swf \
		$(CURDIR)/allowdomain/as3/allowdomain-localhost-as3.as
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/allowdomain/as3/allowinsecuredomain-localhost-as3.swf \
		$(CURDIR)/allowdomain/as3/allowinsecuredomain-localhost-as3.as
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/allowdomain/as3/allowdomain-example-as3.swf \
		$(CURDIR)/allowdomain/as3/allowdomain-example-as3.as
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/allowdomain/as3/allowinsecuredomain-example-as3.swf \
		$(CURDIR)/allowdomain/as3/allowinsecuredomain-example-as3.as

allowdomain-as3-clean:
	$(RMF) $(CURDIR)/allowdomain/as3/*.swf


types-as3:
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/types/as3/bytearray-receiver-as3.swf \
		$(CURDIR)/types/as3/bytearray-receiver-as3.as
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/types/as3/bytearray-sender-as3.swf \
		$(CURDIR)/types/as3/bytearray-sender-as3.as

types-as3-clean:
	$(RMF) $(CURDIR)/types/as3/*.swf


allversions-as2:
	$(MAKESWF) -v 5 \
		-o $(CURDIR)/allversions/as2/connect-swf5-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf5-as2.as
	$(MAKESWF) -v 6 \
		-o $(CURDIR)/allversions/as2/connect-swf6-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf6-as2.as
	$(MAKESWF) -v 7 \
		-o $(CURDIR)/allversions/as2/connect-swf7-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf7-as2.as
	$(MAKESWF) -v 8 \
		-o $(CURDIR)/allversions/as2/connect-swf8-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf8-as2.as
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf9-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf9-as2.as
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf10-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf10-as2.as
	$(EDITSWFV) 10 $(CURDIR)/allversions/as2/connect-swf10-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf11-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf11-as2.as
	$(EDITSWFV) 11 $(CURDIR)/allversions/as2/connect-swf11-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf12-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf12-as2.as
	$(EDITSWFV) 12 $(CURDIR)/allversions/as2/connect-swf12-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf13-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf13-as2.as
	$(EDITSWFV) 13 $(CURDIR)/allversions/as2/connect-swf13-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf14-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf14-as2.as
	$(EDITSWFV) 14 $(CURDIR)/allversions/as2/connect-swf14-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf15-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf15-as2.as
	$(EDITSWFV) 15 $(CURDIR)/allversions/as2/connect-swf15-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf16-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf16-as2.as
	$(EDITSWFV) 16 $(CURDIR)/allversions/as2/connect-swf16-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf17-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf17-as2.as
	$(EDITSWFV) 17 $(CURDIR)/allversions/as2/connect-swf17-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf18-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf18-as2.as
	$(EDITSWFV) 18 $(CURDIR)/allversions/as2/connect-swf18-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf19-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf19-as2.as
	$(EDITSWFV) 19 $(CURDIR)/allversions/as2/connect-swf19-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf20-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf20-as2.as
	$(EDITSWFV) 20 $(CURDIR)/allversions/as2/connect-swf20-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf21-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf21-as2.as
	$(EDITSWFV) 21 $(CURDIR)/allversions/as2/connect-swf21-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf22-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf22-as2.as
	$(EDITSWFV) 22 $(CURDIR)/allversions/as2/connect-swf22-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf23-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf23-as2.as
	$(EDITSWFV) 23 $(CURDIR)/allversions/as2/connect-swf23-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf24-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf24-as2.as
	$(EDITSWFV) 24 $(CURDIR)/allversions/as2/connect-swf24-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf25-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf25-as2.as
	$(EDITSWFV) 25 $(CURDIR)/allversions/as2/connect-swf25-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf26-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf26-as2.as
	$(EDITSWFV) 26 $(CURDIR)/allversions/as2/connect-swf26-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf27-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf27-as2.as
	$(EDITSWFV) 27 $(CURDIR)/allversions/as2/connect-swf27-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf28-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf28-as2.as
	$(EDITSWFV) 28 $(CURDIR)/allversions/as2/connect-swf28-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf29-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf29-as2.as
	$(EDITSWFV) 29 $(CURDIR)/allversions/as2/connect-swf29-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf30-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf30-as2.as
	$(EDITSWFV) 30 $(CURDIR)/allversions/as2/connect-swf30-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf31-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf31-as2.as
	$(EDITSWFV) 31 $(CURDIR)/allversions/as2/connect-swf31-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf32-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf32-as2.as
	$(EDITSWFV) 32 $(CURDIR)/allversions/as2/connect-swf32-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf33-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf33-as2.as
	$(EDITSWFV) 33 $(CURDIR)/allversions/as2/connect-swf33-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf34-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf34-as2.as
	$(EDITSWFV) 34 $(CURDIR)/allversions/as2/connect-swf34-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf35-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf35-as2.as
	$(EDITSWFV) 35 $(CURDIR)/allversions/as2/connect-swf35-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf36-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf36-as2.as
	$(EDITSWFV) 36 $(CURDIR)/allversions/as2/connect-swf36-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf37-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf37-as2.as
	$(EDITSWFV) 37 $(CURDIR)/allversions/as2/connect-swf37-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf38-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf38-as2.as
	$(EDITSWFV) 38 $(CURDIR)/allversions/as2/connect-swf38-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf39-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf39-as2.as
	$(EDITSWFV) 39 $(CURDIR)/allversions/as2/connect-swf39-as2.swf
	$(MAKESWF) -v 9 \
		-o $(CURDIR)/allversions/as2/connect-swf40-as2.swf \
		$(CURDIR)/allversions/as2/connect-swf40-as2.as
	$(EDITSWFV) 40 $(CURDIR)/allversions/as2/connect-swf40-as2.swf

allversions-as2-clean:
	$(RMF) $(CURDIR)/allversions/as2/*.pp
	$(RMF) $(CURDIR)/allversions/as2/*.swf

allversions-as3:
	$(AS3COMP) -T 9 \
		-o $(CURDIR)/allversions/as3/connect-swf9-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf9-as3.as
	$(AS3COMP) -T 10 \
		-o $(CURDIR)/allversions/as3/connect-swf10-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf10-as3.as
	$(AS3COMP) -T 11 \
		-o $(CURDIR)/allversions/as3/connect-swf11-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf11-as3.as
	$(AS3COMP) -T 12 \
		-o $(CURDIR)/allversions/as3/connect-swf12-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf12-as3.as
	$(AS3COMP) -T 13 \
		-o $(CURDIR)/allversions/as3/connect-swf13-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf13-as3.as
	$(AS3COMP) -T 14 \
		-o $(CURDIR)/allversions/as3/connect-swf14-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf14-as3.as
	$(AS3COMP) -T 15 \
		-o $(CURDIR)/allversions/as3/connect-swf15-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf15-as3.as
	$(AS3COMP) -T 16 \
		-o $(CURDIR)/allversions/as3/connect-swf16-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf16-as3.as
	$(AS3COMP) -T 17 \
		-o $(CURDIR)/allversions/as3/connect-swf17-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf17-as3.as
	$(AS3COMP) -T 18 \
		-o $(CURDIR)/allversions/as3/connect-swf18-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf18-as3.as
	$(AS3COMP) -T 19 \
		-o $(CURDIR)/allversions/as3/connect-swf19-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf19-as3.as
	$(AS3COMP) -T 20 \
		-o $(CURDIR)/allversions/as3/connect-swf20-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf20-as3.as
	$(AS3COMP) -T 21 \
		-o $(CURDIR)/allversions/as3/connect-swf21-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf21-as3.as
	$(AS3COMP) -T 22 \
		-o $(CURDIR)/allversions/as3/connect-swf22-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf22-as3.as
	$(AS3COMP) -T 23 \
		-o $(CURDIR)/allversions/as3/connect-swf23-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf23-as3.as
	$(AS3COMP) -T 24 \
		-o $(CURDIR)/allversions/as3/connect-swf24-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf24-as3.as
	$(AS3COMP) -T 25 \
		-o $(CURDIR)/allversions/as3/connect-swf25-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf25-as3.as
	$(AS3COMP) -T 26 \
		-o $(CURDIR)/allversions/as3/connect-swf26-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf26-as3.as
	$(AS3COMP) -T 27 \
		-o $(CURDIR)/allversions/as3/connect-swf27-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf27-as3.as
	$(AS3COMP) -T 28 \
		-o $(CURDIR)/allversions/as3/connect-swf28-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf28-as3.as
	$(AS3COMP) -T 29 \
		-o $(CURDIR)/allversions/as3/connect-swf29-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf29-as3.as
	$(AS3COMP) -T 30 \
		-o $(CURDIR)/allversions/as3/connect-swf30-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf30-as3.as
	$(AS3COMP) -T 31 \
		-o $(CURDIR)/allversions/as3/connect-swf31-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf31-as3.as
	$(AS3COMP) -T 32 \
		-o $(CURDIR)/allversions/as3/connect-swf32-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf32-as3.as
	$(AS3COMP) -T 33 \
		-o $(CURDIR)/allversions/as3/connect-swf33-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf33-as3.as
	$(AS3COMP) -T 34 \
		-o $(CURDIR)/allversions/as3/connect-swf34-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf34-as3.as
	$(AS3COMP) -T 35 \
		-o $(CURDIR)/allversions/as3/connect-swf35-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf35-as3.as
	$(AS3COMP) -T 36 \
		-o $(CURDIR)/allversions/as3/connect-swf36-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf36-as3.as
	$(AS3COMP) -T 37 \
		-o $(CURDIR)/allversions/as3/connect-swf37-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf37-as3.as
	$(AS3COMP) -T 38 \
		-o $(CURDIR)/allversions/as3/connect-swf38-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf38-as3.as
	$(AS3COMP) -T 39 \
		-o $(CURDIR)/allversions/as3/connect-swf39-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf39-as3.as
	$(AS3COMP) -T 40 \
		-o $(CURDIR)/allversions/as3/connect-swf40-as3.swf \
		$(CURDIR)/allversions/as3/connect-swf40-as3.as

allversions-as3-clean:
	$(RMF) $(CURDIR)/allversions/as3/*.swf
