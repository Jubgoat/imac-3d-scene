package contact;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.BeforeEach;
import static org.junit.jupiter.api.Assertions.*;

public class ContactServiceTest {
    private ContactService contactService;

    @BeforeEach
    public void setUp() {
        contactService = new ContactService();
    }

    @Test
    public void testAddContact() {
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        contactService.addContact(contact);
        
        Contact retrieved = contactService.getContact("1");
        assertNotNull(retrieved);
        assertEquals("John", retrieved.getFirstName());
    }

    @Test
    public void testAddDuplicateContact() {
        Contact contact1 = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        Contact contact2 = new Contact("1", "Jane", "Smith", "0987654321", "456 Oak Ave");
        
        contactService.addContact(contact1);
        assertThrows(IllegalArgumentException.class, () -> contactService.addContact(contact2));
    }

    @Test
    public void testAddNullContact() {
        assertThrows(IllegalArgumentException.class, () -> contactService.addContact(null));
    }

    @Test
    public void testDeleteContact() {
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        contactService.addContact(contact);
        
        assertNotNull(contactService.getContact("1"));
        contactService.deleteContact("1");
        assertNull(contactService.getContact("1"));
    }

    @Test
    public void testDeleteNonExistentContact() {
        assertThrows(IllegalArgumentException.class, () -> contactService.deleteContact("999"));
    }

    @Test
    public void testUpdateContact() {
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        contactService.addContact(contact);
        
        contactService.updateContact("1", "Jane", "Smith", "0987654321", "456 Oak Ave");
        
        Contact updated = contactService.getContact("1");
        assertEquals("Jane", updated.getFirstName());
        assertEquals("Smith", updated.getLastName());
        assertEquals("0987654321", updated.getPhone());
        assertEquals("456 Oak Ave", updated.getAddress());
    }

    @Test
    public void testUpdatePartialFields() {
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        contactService.addContact(contact);
        
        // Update only first name and phone
        contactService.updateContact("1", "Jane", null, "0987654321", null);
        
        Contact updated = contactService.getContact("1");
        assertEquals("Jane", updated.getFirstName());
        assertEquals("Doe", updated.getLastName()); // Should remain unchanged
        assertEquals("0987654321", updated.getPhone());
        assertEquals("123 Main St", updated.getAddress()); // Should remain unchanged
    }

    @Test
    public void testUpdateNonExistentContact() {
        assertThrows(IllegalArgumentException.class, () -> {
            contactService.updateContact("999", "Jane", "Smith", "0987654321", "456 Oak Ave");
        });
    }

    @Test
    public void testUpdateWithInvalidData() {
        Contact contact = new Contact("1", "John", "Doe", "1234567890", "123 Main St");
        contactService.addContact(contact);
        
        // This should throw an exception due to invalid phone format
        assertThrows(IllegalArgumentException.class, () -> {
            contactService.updateContact("1", "Jane", "Smith", "invalid", "456 Oak Ave");
        });
    }
}