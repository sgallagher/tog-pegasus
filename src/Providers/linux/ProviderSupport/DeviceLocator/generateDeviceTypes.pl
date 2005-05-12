#!/usr/bin/perl
#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
#//
#// Author: Christopher Neufeld <neufeld@linuxcare.com>
#//         David Kennedy       <dkennedy@linuxcare.com>
#//
#// Modified By: David Kennedy       <dkennedy@linuxcare.com>
#//              Christopher Neufeld <neufeld@linuxcare.com>
#//              Al Stone            <ahs3@fc.hp.com>
#//
#//%////////////////////////////////////////////////////////////////////////////
#
# This file parses pci.ids and generates the DeviceTypes.h file
# 
use Time::localtime;

$pcifile="/usr/share/pci.ids";
$pegasusfile="pegasus.ids";

$now=ctime();

print "/* This file was automatically generated\n";
print " * using $0 from $pcifile\n";
print " * This was performed on $now\n";
print " * Do not edit by hand\n";
print " */\n";

# Do the PCI file first
open(PCIFILE,"<$pcifile");
while($curLine=<PCIFILE>){
	chomp $curLine;

	# Cleanse the string 
	$curLine=~s/-/ /g;
	$curLine=~s/\// /g;
	$curLine=~s/\\/ /g;
	$curLine=~s/\(/ /g;
	$curLine=~s/\)/ /g;
	if($curLine=~m/^C\s+(\S+)\s+(.*)$/){
		$level1=generateLevel1Define($2,$1,'0x00');
		$lineStatus=1;
	}
	elsif($lineStatus==1){
		if($curLine=~m/^\s{1}(\S+)\s+(.*)$/){
			$level2=generateLevel2Define($2,$1,$level1,'0x00');
		}
		elsif($curLine=~m/^\s{2}(\S+)\s+(.*)$/){
			generateLevel3Define($2,$1,$level1,$level2,'0x00');
		}
	}
}
close(PCIFILE);
# Do the pegasus file next
open(PEGASUS_FILE,"<$pegasusfile");
while($curLine=<PEGASUS_FILE>){
	chomp $curLine;

	# Cleanse the string 
	$curLine=~s/-/ /g;
	$curLine=~s/\// /g;
	$curLine=~s/\\/ /g;
	if($curLine=~m/^C\s+(\S+)\s+(.*)$/){
		$level1=generateLevel1Define($2,$1,'0xff');
		$lineStatus=1;
	}
	elsif($lineStatus==1){
		if($curLine=~m/^\s{1}(\S+)\s+(.*)$/){
			$level2=generateLevel2Define($2,$1,$level1,'0xff');
		}
		elsif($curLine=~m/^\s{2}(\S+)\s+(.*)$/){
			generateLevel3Define($2,$1,$level1,$level2,'0xff');
		}
	}
}
close(PEGASUS_FILE);

sub generateLevel1Define{
	my($name,$value,$prefix)=@_;
	my($retval);

	@words=split(/\s/,$name);
	print "#define Device_";
	foreach $iword (@words){
		print ucfirst $iword;
		$retval.=ucfirst $iword;
	}

	print " $prefix$value\n";
	return $retval;
}

sub generateLevel2Define{
	my($name,$value,$level1,$prefix)=@_;
	my($retval);

	@words=split(/\s/,$name);
	print "#define Device_".$level1."_";
	foreach $iword (@words){
		print ucfirst $iword;
		$retval.=ucfirst $iword;
	}

	print " $prefix$value\n";
	return $retval;
}

sub generateLevel3Define{
	my($name,$value,$level1,$level2,$prefix)=@_;
	my($retval);

	@words=split(/\s/,$name);
	print "#define Device_".$level1."_".$level2."_";
	foreach $iword (@words){
		print ucfirst $iword;
		$retval.=ucfirst $iword;
	}

	print " $prefix$value\n";
	return $retval;
}
