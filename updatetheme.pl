#!/usr/bin/perl

$color_theme = "colors";

sub color {
	$_ = @_[0];
	my $args;
	s/\$([^:]+)/$colors{$1}/;
	if (!$_) {
		$_ = "\$colors{\"@_[0]\"} is not set";
		s/"\$/"/;
		die $_;
	}

	if (/:/) {
		($_, $args) = split(/:/);
		$args =~ s/[rgb]/-$&/;
		$args =~ s/,/ /;
	}
	$_ = `./rgbhex $_ $args`;
	chop $_;
	return $_;
}

sub write_xpmpal {
	my ($files, %pal) = @_;
	my ($char, $color_key, $color);

	while (($char, $color_key) = each(%pal)) {
		$char =~ s/[.\/^]/\\$&/;
		$color = color("\$$color_key");
		system("sed -i 's/\\($char\tc #\\)[^\"]*/\\1$color/' $files");
	}
}

sub write_themevars {
	my ($color_key, $color);
	foreach $color_key (@_) {
		$_ = $color_key;
		s/\[(\d)\]/_$1/;
		system ("sed -i 's/^\\(\$$_ = \\).*/\\1\"#"
			. color("\$$color_key")
			. "\"/i' theme");
	}
}

open(COLORS, $color_theme) || die "Cannot open file: $color_theme\n";
foreach $line (<COLORS>) {
	$_ = $line;
	s/#.*$//;
	s/^\s+//;
	s/\s+$//;

	if (m/./) {
		($key, $val) = split(/\s*=\s*/);
		$_ = $val;		
		if (/\s/) {
			$i = 0;
			foreach $val (split(/\s+/)) {
				$colors{"$key\[$i\]"} = color($val);
				$i++;
			}
		}
		else {
			$colors{$key} = color($val);
		}
	}
}
close(COLORS);

write_xpmpal(
	"top*.xpm left-*.xpm right-*.xpm btm*.xpm",
	' ', "BorderOutline_fo[1]",
	'.', "BorderOutline_fo[0]",
	'+', "BorderOutline_un[1]",
	'@', "BorderOutline_un[0]",
	'#', "BorderShadow",
	'$', "Border",
	'%', "BorderLight");

write_xpmpal(
	"{close,max}-{fo,pr}.xpm {title,sep}-fo.xpm",
	' ', "TitleOutline_fo[1]",
	'´', "TitleOutline_fo[0]",
	'{', "TitleBG_fo[0]",
	'%', "TitleBG_fo[1]",
	'@', "TitleLight_fo[0]",
	'$', "TitleLight_fo[1]");

@files = (
	"close-{fo,pr}.xpm sep-fo.xpm",
	"max-{fo,pr}.xpm");

for ($i = 0; $i < 2; $i++) {
	write_xpmpal(
		$files[$i],
		'`', "TitleShadow_fo[$i]",
		'.', "TitleShadow_fo[$i]:15%",
		';', "TitleShadow_fo[$i]:30%",
		'!', "TitleShadow_fo[$i]:37%",
		'&', "TitleBG_fo[$i]:5%");
}

write_xpmpal(
	"{close,max,title,sep}-un.xpm",
	' ', "TitleOutline_un[1]",
	'´', "TitleOutline_un[0]",
	'.', "TitleShadow_un[0]",
	';', "TitleShadow_un[1]",
	'/', "TitleBG_un",
	'@', "TitleLight_un[0]",
	'#', "TitleLight_un[1]");

write_themevars(
	"TitleText_fo[0]",
	"TitleText_fo[1]",
	"TitleText_un[0]",
	"TitleText_un[1]");

write_xpmpal(
	"arrow-*.xpm",
	' ', "MenuShadow",
	'.', "MenuSelectedBG",
	'+', "MenuBG_un",
	':', "MenuBG_fo",
	'$', "MenuSelectedLight",
	'@', "MenuLight");

write_themevars(
	"MenuBG_fo",
	"MenuBG_un",
	"MenuLight",
	"MenuShadow",
	"MenuText",
	"MenuSelectedBG",
	"MenuSelectedText");

write_themevars(
	"PanelBG",
	"PanelLight",
	"PanelShadow",
	"PanelText");
