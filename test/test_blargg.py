import subprocess
import os

binary = "./bin/gbe"
gbdoctor_path = "../tools/gameboy-doctor/"
logfile = "../../build/cpu_log.txt"

base_path = "../tools/gb-test-roms/cpu_instrs/individual/"
cartridges = [
    "01-special.gb",
    "02-interrupts.gb",
    "03-op sp,hl.gb",
    "04-op r,imm.gb",
    "05-op rp.gb",
    "06-ld r,r.gb",
    "07-jr,jp,call,ret,rst.gb",
    "08-misc instrs.gb",
    "09-op r,r.gb",
    "10-bit ops.gb",
    "11-op a,(hl).gb",
]

cartridge_path = [base_path + cartridge for cartridge in cartridges]

cur_path = os.getcwd()


os.chdir("../build/")
popen = subprocess.Popen(["make"], stdout=subprocess.PIPE)
popen.wait()
output = popen.stdout.read()
print(output.decode())

for idx, path in enumerate(cartridge_path):
    # if idx <= 6:
    #     continue

    print(f"Running {cartridges[idx]}...", end='')

    
    args = (binary, path)
    popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    popen.wait()
    output = popen.stdout.read()
    print(output)
    print(output.decode().strip().split('\n')[-1])
    continue

    # args = ("./gameboy-doctor", logfile, "cpu_instrs", str(idx + 1))
    # os.chdir(cur_path + "/" + gbdoctor_path)
    # popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    # popen.wait()
    # output = popen.stdout.read()
    # if popen.returncode != 0:
    #     print(output.decode())
    #     break

# cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/01-special.gb"; // OK!
# cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/02-interrupts.gb"; // Need timer interrupt
# 03 // OK
# // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/04-op r,imm.gb"; // OK!
# // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/05-op rp.gb"; // OK!
# // cartridge_path = "../tools/gb-test-roms/cpu_instrs/individual/06-ld r,r.gb"; // OK!
# 07 - missing banks // OK
# 08 // OK
# 09 // Missing SCF
# 10 // OK but need to run longer?
# 11 // Missing LD A, (bc)
