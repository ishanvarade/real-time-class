watch -n 0.1 "dmesg | grep "DequeueOverHead" | tail -n 1 >> abc.txt"


1.
for i in {0..11} ; do sudo cpufreq-set -c ${i} -g userspace ; done

for i in {0..11} ; do sudo cpufreq-info -c ${i} -w ; done

2.
./Desktop/log_file.sh

3.
cd git/linux-4.9.57-i/linux-4.9.57-i/
rm -vf linux*.deb ; rm -vf linux*.dsc ; rm -vf linux*.changes ; rm -vf linux*.gz; ls

4. 
time make -j 12 KDEB_PKGVERSION=4.9.57.1 deb-pkg ; echo -ne '\007'

5. 
sudo dpkg -i ../linux*.deb ; echo -ne '\007'

echo -ne '\007'






cd ; cd git/linux-4.9.57-i/linux-4.9.57-i/; rm -vf linux*.deb ; rm -vf linux*.dsc ; rm -vf linux*.changes ; rm -vf linux*.gz; ls; cd linux-4.9.57/; time make -j 12 KDEB_PKGVERSION=4.9.57.1 deb-pkg ; echo -ne '\007'



cd ; cd git/linux-4.9.57-i/linux-4.9.57-i/; rm -vf linux*.deb ; rm -vf linux*.dsc ; rm -vf linux*.changes ;  ls; cd linux-4.9.57/; time make -j 12 KDEB_PKGVERSION=4.9.57.1 deb-pkg ; echo -ne '\007'


>
watch -n 0.1 "cat /proc/interrupts | tail -n  $((LINES-50))"

>
watch -n 0.1 "dmesg | tail -n  $((LINES-50))"