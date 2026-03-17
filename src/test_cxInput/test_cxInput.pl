#!/usr/bin/perl -w

use strict;

my $BACK = "";
my $cF2 = "O5Q";
my $DOWN = "OB";
my $END = "OF";
my $ESC ="";
my $F10 = "[21~";
my $F11 = "[23~";
my $F12 = "[24~";
my $F1 = "OP";
my $F2 = "OQ";
my $F3 = "OR";
my $F4 = "OS";
my $F5 = "[15~";
my $F6 = "[17~";
my $F7 = "[18~";
my $F8 = "[19~";
my $F9 = "[20~";
my $HOME = "OH";
my $LEFT = "OD";
my $PAGEDOWN = "[6~";
my $PAGEUP = "[5~";
my $RETURN = "
";
my $RIGHT = "OC";
my $sF11 = "[23;2~";
my $sF1 = "O2P";
my $sF2 = "O2Q";
my $sF9 = "[20;2~";
my $SHIFT_TAB = "[Z";
my $TAB = "	"; # Note: There really is a tab in the quotes..
my $UP = "OA";


use Expect;
my $exp = new Expect or die "Warning: Could not create the expect object.";

my $exe = "./test_cxInput";

if ($#ARGV == 1) {
   my $label = $ARGV[0];
   my $value = $ARGV[1];

   $value =~ s/#HOME#/${HOME}/g;
   $value =~ s/#END#/${END}/g;
   $value =~ s/#BACK#/${BACK}/g;
   $value =~ s/#LEFT#/${LEFT}/g;
   $value =~ s/#RIGHT#/${RIGHT}/g;

   unlink("test_cxInput.log") if -f "test_cxInput.log";
   $exp->log_stdout(0);
   $exp->spawn($exe) or die "Warning: Could not run ($exe)";
   # Wait for the process to start and ncurses to initialize
   # If we don't wait, the first test's output might be lost or the process might not be ready for input
   select(undef, undef, undef, 0.2);
   $exp->expect(1, 
      [ qr/$label/ =>
         sub {
            $exp->send("$value\r");
            exp_continue;
         }
      ],
   );
   $exp->soft_close();
   # Wait a bit for the process to finish writing the log
   for (my $i=0; $i<10; $i++) {
      last if -f "test_cxInput.log";
      select(undef, undef, undef, 0.05);
   }
}

