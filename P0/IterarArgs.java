public class IterarArgs {
    public static void main(String[] args) {
        System.out.println("Se detectaron " + args.length + " argumentos:");

        for (int i = 0; i < args.length; i++) {
	    int count = i+1;
            System.out.println("Argumento [" + count + "]: " + args[i]);
        }
    }
}
