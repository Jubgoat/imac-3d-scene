package contact;

public class Main {
    public static void main(String[] args) {
        ContactService contactService = new ContactService();
        
        // Add some contacts
        Contact contact1 = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        Contact contact2 = new Contact("2", "Jane", "Smith", "0987654321", "456 Oak Ave");
        
        contactService.addContact(contact1);
        contactService.addContact(contact2);
        
        System.out.println("=== All Contacts ===");
        System.out.println("Contact 1: " + contactService.getContact("1").getFirstName());
        System.out.println("Contact 2: " + contactService.getContact("2").getFirstName());
        
        // Update a contact
        contactService.updateContact("1", "Johnny", null, null, null);
        System.out.println("\n=== After Update ===");
        System.out.println("Contact 1: " + contactService.getContact("1").getFirstName());
        
        // Delete a contact
        contactService.deleteContact("2");
        System.out.println("\n=== After Delete ===");
        System.out.println("Contact 2: " + (contactService.getContact("2") == null ? "Not Found" : "Found"));
    }
}
