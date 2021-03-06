#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//get the size of Attribute element in Struct
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
//get the offset of Attribute element in Struct
#define offset_of_attribute(Struct, Attribute) (unsigned long)&((Struct*)0)->Attribute
#define COLUM_USERNAME_SIZE 32
#define COLUM_EMIAL_SIZE 255


enum ExecuteResult_t { EXECUTE_SUCCESS, EXECUTE_TABLE_FULL };
typedef enum ExecuteResult_t ExecuteResult;

enum MetaCommandResult_t {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
};

enum PrepareResult_t {
  PREPARE_SUCCESS,
  PREPARE_UNRECOGNIZED_STATEMENT,
  PREPARE_SYNTAX_ERROR,
};

struct InputBuffer_t {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
};

enum StatementType_t
{
  STATEMENT_INSERT,
  STATEMENT_SELECT,
  STATEMENT_UNRECOGNIZED
};
typedef enum StatementType_t StatementType;

struct Row_t
{
    int id;
    char username[COLUM_USERNAME_SIZE];
    char email[COLUM_EMIAL_SIZE];
};
typedef struct Row_t Row;

struct Statement_t
{
  StatementType type;
  Row row_to_insert;
};

typedef struct InputBuffer_t InputBuffer;
typedef enum MetaCommandResult_t MetaCommandResult;
typedef enum PrepareResult_t PrepareResult;
typedef struct Statement_t Statement;


const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = offset_of_attribute(Row, id);
const uint32_t USERNAME_OFFSET = offset_of_attribute(Row, username);
const uint32_t EMAIL_OFFSET = offset_of_attribute(Row, email);
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t TABLE_MAX_PAGES = 1000;

//const uint32_t TABLE_MAX_PAGES = 100;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;


//table structure
struct Table_t
{
    //declare a pointer point to the pages in the table
    void* pages[TABLE_MAX_PAGES];
    uint32_t num_rows;
};
typedef struct Table_t Table;

//print a row' detail infomation in the table
void print_row(Row* row)
{
    if(row == NULL)
    {
        printf("this is row is invalid!");
        return;
    }
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

//TODO use memory pool to replace malloc
//create new input buffer
InputBuffer* new_input_buffer()
{
    InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
    if(input_buffer == NULL)
    {
        printf("malloc out of space");
        exit(1);    
    }
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

//create a new table
Table* new_table()
{
    Table* table = malloc(sizeof(Table));
    if(table == NULL)
    {
        printf("malloc out of space");
        exit(1);
    }
    table->num_rows = 0;
    return table;
}

//db shell display
void print_prompt() { printf("db > "); }

void read_input(InputBuffer* input_buffer) {
  ssize_t bytes_read =
      getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  // Ignore trailing newline
  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer)
{
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement)
{
  if(strncmp(input_buffer->buffer, "insert", 6) == 0)
  {
    statement->type = STATEMENT_INSERT;
    int args_assigned = sscanf(
        input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id),
        statement->row_to_insert.username, statement->row_to_insert.email);
    if(args_assigned < 3)
        return PREPARE_SYNTAX_ERROR;
    return PREPARE_SUCCESS;
  }
  else if(strncmp(input_buffer->buffer, "select", 6) == 0)
  {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }
  else {
    statement->type = STATEMENT_UNRECOGNIZED;
    return PREPARE_UNRECOGNIZED_STATEMENT;
  }
}

/*
void execute_statement(Statement* statement)
{
  switch(statement->type)
  {
    case (STATEMENT_INSERT):
      printf("This is where we would do an insert.\n");
      break;
    case (STATEMENT_SELECT):
      printf("This is where we would do a select.\n");
      break;
    case (STATEMENT_UNRECOGNIZED):
      printf("Unrecognized operation! \n");
      break;
  }
}
*/

//serialize and deserialize in order to save memory
void serialize_row(Row* source, void* destination)
{
    memcpy(destination+ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination+USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination+EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination)
{
    memcpy(&(destination->id), source+ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source+USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source+EMAIL_OFFSET, EMAIL_SIZE);
}

//TODO add test
void* row_slot(Table* table, uint32_t row_num)
{
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = table->pages[page_num];
    if (!page) {
        // Allocate memory only when we try to access page
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}


ExecuteResult execute_insert(Statement* statement, Table* table) {
  if (table->num_rows >= TABLE_MAX_ROWS) {
    return EXECUTE_TABLE_FULL;
  }

  Row* row_to_insert = &(statement->row_to_insert);

  serialize_row(row_to_insert, row_slot(table, table->num_rows));
  table->num_rows += 1;

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table) {
  Row row;
  uint32_t i;
  for (i = 0; i < table->num_rows; i++) {
    deserialize_row(row_slot(table, i), &row);
    print_row(&row);
 }
  return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement* statement, Table* table)
{
    switch (statement->type) {
        case (STATEMENT_INSERT):
            return execute_insert(statement, table);
        case (STATEMENT_SELECT):
            return execute_select(statement, table);
    }
}



int main(int argc, char* argv[]) {
    Table* table = new_table();
    InputBuffer* input_buffer = new_input_buffer();
    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        if(input_buffer->buffer[0] == '.')
        {
            switch(do_meta_command(input_buffer))
            {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch(prepare_statement(input_buffer, &statement))
        {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_SYNTAX_ERROR):
                printf("Syntax error. Could not parse statement\n");
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("unrecognized keyword at start of '%s' .\n", input_buffer->buffer);
                break;
        }
        //execute_statement(&statement);
        //printf("Executed.\n");
        switch (execute_statement(&statement, table))
        {
            case (EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case (EXECUTE_TABLE_FULL):
                printf("Error: Table full.\n");
                break;
        }
    }
}
