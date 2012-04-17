# SILC

A compiler for the *Simple Integer Language (SIL)* that reads a SIL source program, and generates instructions for a target machine known as the *Simple Integer Machine (SIM)*.  
Source Language (SIL) description can be found at [http://athena.nitc.ac.in/~kmurali/Compiler/sil.html]  
Target Machine (SIM) Architecture description can be found at [http://athena.nitc.ac.in/~kmurali/Compiler/sim.html]

## Author :
* Addy Singh <addy689@gmail.com>

## Contributors :
* Saurabh Sinha <saurabh.nitc10@gmail.com>
* Vipin Nair <swvist@gmail.com>

## License :
* The code is licensed under the MIT License

<br/>
# Repository Information

### Folder Listing
<table>
	<tr>
		<th>Folder</th><th>Description</th>
	</tr>
	
	<tr>
		<td>SILCompiler</td><td>Contains compiler source programs that are used to compile and generate SIM code for a SIL source program</td>
	</tr>
	
	<tr>
		<td>SILCompiler/LexYaccFiles</td><td>Contains a Lex file <i>SIL.l</i> (for lexically analysing SIL source program) and a Yacc file <i>SIL.y</i> (for parsing SIL source program)</td>
	</tr>
	
	<tr>
		<td>Main</td><td>Contains a <i>Makefile</i> for compiling the compiler source programs, SIL source program files</td>
	</tr>
	
	<tr>
		<td>Main/SIM_Simulator</td><td>Contains the simulator for SIM, and a file <i>SIMcode</i> (target file for the SIM instructions that the compiler generates for an input SIL program)</td>
	</tr>

</table>

### Instructions
1. Compile the compiler source programs using the <b>Makefile</b> present in <i>Main/</i>. In the terminal, write
	<pre>
	<code>$ cd Main/</code><br />
	<code>$ make all</code>
	</pre>


2. Using the object file <b>sil</b> that is created, compile any of the SIL source programs present in <i>Main/</i>. In the terminal, write
	<pre>
	<code>$ ./sil SILsource1</code>
	</pre>

3. The SIL source program will be executed. Also, the SIM instructions that are generated will be stored in file <b>SIMcode</b>

4. The instructions in the file <b>SIMcode</b> are executed using the SIM simulator present in <i>Main/SIM_Simulator/</i>. To execute the generated SIM instructions, write in the terminal
	<pre>
	<code>$ cd SIM_Simulator/</code><br/>
	<code>$ make all</code><br/>
	<code>$ ./sim ../SIMcode</code>
	</pre>

