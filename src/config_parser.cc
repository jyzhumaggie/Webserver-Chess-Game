// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <features.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/log/trivial.hpp>

#include "config_parser.h"

std::string NginxConfig::ToString(int depth) {
	std::string serialized_config;
	for (const auto& statement : statements_) {
		serialized_config.append(statement->ToString(depth));
	}
	return serialized_config;
}

// Gets the port number from parsed config and returns the first valid port (in case there are many) and -1 if there is an error
int NginxConfig::get_port_from_config(const NginxConfig* config) {
	if (config == nullptr)
	{
			return -1;
	}

	//traversing statements without child blocks
	for (auto pStatement : config -> statements_)
	{
		if (pStatement->child_block_.get() == nullptr)
		{
			if (pStatement->tokens_[0] == "port" && pStatement->tokens_.size() == 2)
			{
				int ret = atoi(pStatement->tokens_[1].c_str());
				return (ret >= 0 && ret <= 0xffff) ? ret : -1; //checks for invalid range on port number
			}
		}
	}
		
	// Now traversing statements with child blocks
	for (auto pStatement : config -> statements_)
	{
		if (pStatement->child_block_.get() != nullptr)
		{
			int ret;
			if ((ret = get_port_from_config(pStatement->child_block_.get())) != -1)
			{
				return ret;
			}
		}
	}
		//error case
	return -1;
}

std::vector<std::string> NginxConfig::get_handler_types() {
	for (auto s : statements_) {
		if (s->tokens_[0] == "server" && s->child_block_.get() != nullptr) {
			for (auto child : s->child_block_->statements_) {
				if (child->tokens_[0] == "location" &&
					child->tokens_.size() >= 3 &&
					child->child_block_.get() != nullptr) {
						std::vector<std::string>::iterator it = std::find(handler_factory_types_.begin(), handler_factory_types_.end(), child->tokens_[2]);
						if (it == handler_factory_types_.end()) {
							handler_factory_types_.push_back(child->tokens_[2]);
						}
				}
			}
		}
	}
	return handler_factory_types_;
}
std::vector<path> NginxConfig::get_path_from_config() {
	for (auto statement : statements_)
  	{
		if (statement->tokens_[0] == "server" &&
			statement->child_block_.get() != nullptr)
		{
			for (auto child_statement : statement->child_block_->statements_)
			{
				if (child_statement->tokens_[0] == "location" && 
					child_statement->tokens_.size() >= 2 && 
					child_statement->child_block_.get() != nullptr)
				{
					if (child_statement->tokens_.size() == 2) {
						auto s = child_statement->child_block_->statements_;
						if (s.size() == 0) {
							path current_path;
							current_path.endpoint = child_statement->tokens_[1];
							BOOST_LOG_TRIVIAL(info) << "Getting echo path: " << current_path.endpoint;
							paths_.push_back(current_path);
						}

					}
					for (auto location_statement : child_statement->child_block_->statements_)
					{
						if (location_statement->tokens_[0] == "root" && 
							location_statement->tokens_.size() >= 2 && 
							child_statement->child_block_.get() != nullptr)         
						{
							path current_path;
                            current_path.endpoint = child_statement->tokens_[1];
							current_path.root = location_statement->tokens_[1];
							BOOST_LOG_TRIVIAL(info) << "Getting static path: " << current_path.endpoint;
							paths_.push_back(current_path);
						}
						
					}        
				}

			}
		} 
	}
	return paths_;
}




std::string NginxConfigStatement::ToString(int depth) {
	std::string serialized_statement;
	for (int i = 0; i < depth; ++i) {
		serialized_statement.append("  ");
	}
	for (unsigned int i = 0; i < tokens_.size(); ++i) {
		if (i != 0) {
			serialized_statement.append(" ");
		}
		serialized_statement.append(tokens_[i]);
	}
	if (child_block_.get() != nullptr) {
		serialized_statement.append(" {\n");
		serialized_statement.append(child_block_->ToString(depth + 1));
		for (int i = 0; i < depth; ++i) {
			serialized_statement.append("  ");
		}
		serialized_statement.append("}");
	} else {
		serialized_statement.append(";");
	}
	serialized_statement.append("\n");
	return serialized_statement;
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
	switch (type) {
		case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
		case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
		case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
		case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
		case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
		case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
		case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
		case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
		default:                       return "Unknown token type";
	}
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
																													 std::string* value) {
	TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
	while (input->good()) {
		const char c = input->get();
		if (!input->good()) {
			break;
		}
		switch (state) {
			case TOKEN_STATE_INITIAL_WHITESPACE:
				switch (c) {
					case '{':
						*value = c;
						return TOKEN_TYPE_START_BLOCK;
					case '}':
						*value = c;
						return TOKEN_TYPE_END_BLOCK;
					case '#':
						*value = c;
						state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
						continue;
					case '"':
						*value = c;
						state = TOKEN_STATE_DOUBLE_QUOTE;
						continue;
					case '\'':
						*value = c;
						state = TOKEN_STATE_SINGLE_QUOTE;
						continue;
					case ';':
						*value = c;
						return TOKEN_TYPE_STATEMENT_END;
					case ' ':
					case '\t':
					case '\n':
					case '\r':
						continue;
					default:
						*value += c;
						state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
						continue;
				}
			case TOKEN_STATE_SINGLE_QUOTE:
				*value += c;
				if (c == '\'') {
						const char cNext = input->get();
						if (cNext != '\n' && cNext != ';' && cNext != '\t' && cNext != '{' && cNext != '}' && cNext != ' ')
						{
								return TOKEN_TYPE_ERROR;
						}
						else
						{
								input->unget();
						}            
					return TOKEN_TYPE_NORMAL;
				}
				else if (c=='\\')
				{
						value->pop_back();
						state = TOKEN_STATE_ESCAPE_WITHIN_SINGLE_QUOTE;
				}
				continue;
			case TOKEN_STATE_DOUBLE_QUOTE:
				*value += c;
				if (c == '"') {
						const char cNext = input->get();
						if (cNext != '\n' && cNext != ';' && cNext != '\t' && cNext != '{' && cNext != '}' && cNext != ' ')
						{
								return TOKEN_TYPE_ERROR;
						}
						else
						{
								input->unget();
						}
						return TOKEN_TYPE_NORMAL;
				}
				else if (c=='\\')
				{
						value->pop_back();
						state = TOKEN_STATE_ESCAPE_WITHIN_DOUBLE_QUOTE;
				}
				continue;
			case TOKEN_STATE_TOKEN_TYPE_COMMENT:
				if (c == '\n' || c == '\r') {
					return TOKEN_TYPE_COMMENT;
				}
				*value += c;
				continue;
			case TOKEN_STATE_TOKEN_TYPE_NORMAL:
				if (c == ' ' || c == '\t' || c == '\n' || c == '\t' ||
						c == ';' || c == '{' || c == '}') {
					input->unget();
					return TOKEN_TYPE_NORMAL;
				}
				*value += c;
				continue;
			case TOKEN_STATE_ESCAPE_WITHIN_DOUBLE_QUOTE:
				*value += c;
				state = TOKEN_STATE_DOUBLE_QUOTE;
				continue;
			case TOKEN_STATE_ESCAPE_WITHIN_SINGLE_QUOTE:
				*value += c;
				state = TOKEN_STATE_SINGLE_QUOTE;
				continue;
		}
	}

	// If we get here, we reached the end of the file.
	if (state == TOKEN_STATE_SINGLE_QUOTE ||
			state == TOKEN_STATE_DOUBLE_QUOTE) {
		return TOKEN_TYPE_ERROR;
	}

	return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
	std::stack<NginxConfig*> config_stack;
	config_stack.push(config);
	TokenType last_token_type = TOKEN_TYPE_START;
	TokenType token_type;
	while (true) {
		std::string token;
		token_type = ParseToken(config_file, &token);
		printf ("%s: %s\n", TokenTypeAsString(token_type), token.c_str());
		if (token_type == TOKEN_TYPE_ERROR) {
			break;
		}

		if (token_type == TOKEN_TYPE_COMMENT) {
			// Skip comments.
			continue;
		}

		if (token_type == TOKEN_TYPE_START) {
			// Error.
			break;
		} else if (token_type == TOKEN_TYPE_NORMAL) {
			if (last_token_type == TOKEN_TYPE_START ||
					last_token_type == TOKEN_TYPE_STATEMENT_END ||
					last_token_type == TOKEN_TYPE_START_BLOCK ||
					last_token_type == TOKEN_TYPE_END_BLOCK ||
					last_token_type == TOKEN_TYPE_NORMAL) {
				if (last_token_type != TOKEN_TYPE_NORMAL) {
					config_stack.top()->statements_.emplace_back(
							new NginxConfigStatement);
				}
				config_stack.top()->statements_.back().get()->tokens_.push_back(
						token);
			} else {
				// Error.
				break;
			}
		} else if (token_type == TOKEN_TYPE_STATEMENT_END) {
			if (last_token_type != TOKEN_TYPE_NORMAL) {
				// Error.
				break;
			}
		} else if (token_type == TOKEN_TYPE_START_BLOCK) {
			if (last_token_type != TOKEN_TYPE_NORMAL) {
				// Error.
				break;
			}
			NginxConfig* const new_config = new NginxConfig;
			config_stack.top()->statements_.back().get()->child_block_.reset(
					new_config);
			config_stack.push(new_config);
		} else if (token_type == TOKEN_TYPE_END_BLOCK) {
			if (last_token_type != TOKEN_TYPE_STATEMENT_END && last_token_type != TOKEN_TYPE_END_BLOCK && last_token_type != TOKEN_TYPE_START_BLOCK) {
				// Error.
				break;
			}
			config_stack.pop();
			//to ensure that program returns an error when there are invalid curly braces in config file
			if(config_stack.size()==0)
			{
				break;
			}
		} else if (token_type == TOKEN_TYPE_EOF) {
			if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
					last_token_type != TOKEN_TYPE_END_BLOCK && last_token_type != TOKEN_TYPE_START) {
				// Error.
				break;
			}
			else if (config_stack.size()>1)
			{
				break;
			}
			return true;
		} else {
			// Error. Unknown token.
			break;
		}
		last_token_type = token_type;
	}

	printf ("Bad transition from %s to %s\n",
					TokenTypeAsString(last_token_type),
					TokenTypeAsString(token_type));
	return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
	std::ifstream config_file;
	config_file.open(file_name);
	if (!config_file.good()) {
		BOOST_LOG_TRIVIAL(error) << "ConfigParser: Failed to open config file: " << file_name << "\n";
		return false;
	}

	const bool return_value =
			Parse(dynamic_cast<std::istream*>(&config_file), config);
	config_file.close();
	return return_value;
}